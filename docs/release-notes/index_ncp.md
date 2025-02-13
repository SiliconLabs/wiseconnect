# **WiSeConnect3\_SDK\_3.4.1 NCP Release Notes**   
## **Release Details**

|**Item**|**Details**|
| :-: | :-: |
|Release date|13th February 2025|
|SDK Version|3\.4.1|
|Firmware Version|Standard: 1711.2.13.4.1.0.4|
|GSDK/SiSDK Version|SiSDK 2024.12.1|
|Studio Version|5\.10.2.0|
|Release Package Name|WiSeConnect3\_SDK\_3.4.1|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components
  - Standard Wireless Firmware -  SiWx917 Firmware Binary available as SiWG917-B.2.13.4.1.0.4.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode.

> Note:
> - The release packages will have bug-fixes, enhancements, and new features in both 'SDK' and 'Firmware'. Customer shall update and use 'SDK' and 'Firmware' of same release package. SDK and FW combinations that are not released together are not supported.
## **Supported Hardware OPNs**

|**Hardware**|**OPN (Ordering Part Number)**|
| :-: | :-: |
|IC OPN|<p>QFN OPN: SiWN917M100LGTBA (Wi-Fi 6 NCP IC, QFN 7x7, 2.4 GHz, 4MB stacked flash, -40 to +85C​) </p><p>Module OPN: SiWN917Y100LGNBx</p>|
|Expansion kits:|<p>SiWx917-EB4346A (based on Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)</p><p>Module Board: SiW917Y-RB4357A (SiWN917Y Module Wi-Fi 6 and Bluetooth LE 4MB Flash RF-Pin Co-Processor Radio Board)</p>|
## **Supported Features** 

<table><tr><th><b>Section</b></th><th><b>Sub-Section</b></th><th><b>Feature</b></th></tr>
<tr><td rowspan="9">System</td><td rowspan="5">Operating modes</td><td>Wi-Fi STA (802.11ax, 802.11n)</td></tr>
<tr><td>Wi-Fi 802.11n AP</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + BLE</td></tr>
<tr><td valign="top">Wi-Fi Transceiver (802.11 b/g)</td></tr>
<tr><td>Security </td><td>Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Debug Lock, Flash Protection</td></tr>
<tr><td>Secure firmware upgrade options</td><td><p>- Firmware loading through UART, SPI Interface</p><p>- Secure Over the Air (OTA) Upgrade</p><p>- Firmware update via Bootloader</p></td></tr>
<tr><td>Crypto Support</td><td><p>- Crypto API's for Hardware Accelerators: Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)</p><p>- Software Accelerators: RSA, ECC</p><p>- Wrapping Secret keys (Symmetric crypto). </p><p>- Added ECDSA Sign and Verify APIs</p></td></tr>
<tr><td>System Power Save</td><td><p>- Deep Sleep with RAM retention and without RAM retention. </p><p>- Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. </p></td></tr>
<tr><td rowspan="14">Wi-Fi</td><td>Wi-Fi Protocols</td><td>IEEE 802.11 b/g/n/ax (2.4GHz)</td></tr>
<tr><td>Access Point (AP) Mode</td><td><p>- 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)</p><p>- Wi-Fi Security </p><p>- WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2) </p></td></tr>
<tr><td>Wi-Fi Scan</td><td>Selective Scan, Active/Passive Scan</td></tr>
<tr><td>Wi-Fi STA (Security Modes)</td><td>Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)</td></tr>
<tr><td>WPA2 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td></tr>
<tr><td>WPA3 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td></tr>
<tr><td>Wi-Fi STA Rejoin</td><td></td></tr>
<tr><td>Wi-Fi STA Roaming </td><td>BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication</td></tr>
<tr><td>Wi-Fi Protocol Power Save </td><td><p>- Deep sleep (unconnected state) with/without RAM retention</p><p>- Associated Sleep (Connected sleep) with Max/Fast/Enhanced-Max Power Save Profile(PSP)</p><p>- TWT</p></td></tr>
<tr><td>QoS</td><td>WMM-QoS</td></tr>
<tr><td>Wi-Fi 6 Feature</td><td>MU-MIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID</td></tr>
<tr><td>Wi-Fi Concurrency </td><td>AP+STA (Same channel)</td></tr>
<tr><td>Wi-Fi Band/Channels</td><td>2.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14</td></tr>
<tr><td>Known Security Vulnerabilities Handled</td><td>WPA2 KRACK Attacks, Fragment and Forge Vulnerability</td></tr>
<tr><td rowspan="4">Network stack</td><td>Core Networking Features</td><td><p>- IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6</p><p>- SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 </p><p>- SSL server versions TLSV1.0 and TLSV1.2</p><p>- DHCPv4 Client,DHCPv6 Client</p><p>- DHCPv4 Server,DHCPv6 Server</p><p>- TCP/IP Bypass (LWIP as Hosted stack for reference)</p></td></tr>
<tr><td>Advanced Network Features</td><td>- HTTP Client/HTTPS Client/mDNS/DNS Client/SNTP Client, IGMP, Embedded MQTT, MQTT on host (AWS and AZURE)</td></tr>
<tr><td>Wi-Fi IoT Cloud Integration</td><td><p>- AWS IoT Core</p><p>- Azure IoT Core</p></td></tr>
<tr><td>BSD and IoT sockets application programming interface(API)</td><td></td></tr>
<tr><td rowspan="2">BLE </td><td>Legacy features</td><td><p>- GAP(Advertising, Scanning, initiation, Connection and Bonding)</p><p>- Generic Attribute Protocol(GATT)</p><p>- Attribute protocol(ATT)</p><p>- Security</p><p>- LL Privacy 1.2</p><p>- Accept list</p><p>- Directed Advertising</p><p>- LE PHY(1Mbps, 2Mbps) & Coded PHY(125kbps, 500kbps)</p><p>- Simultaneous scanning on 1Mbps and Coded PHY</p><p>- LE dual role topology</p><p>- LE data packet length extensions(DLE)</p><p>- Asymmetric PHYs</p><p>- LE channel selection algorithm 2 (CSA#2)</p><p>- LE Secure connections</p><p>- Bluetooth 5.4 Qualified</p></td></tr>
<tr><td>Advertising Extensions </td><td><p>- Extended Advertising</p><p>- Periodic Advertising</p><p>- Periodic Advertising scanning</p><p>- Extended Advertising scanning</p><p>- Periodic Advertising list</p><p>- LE periodic advertising synchronization</p></td></tr>
</table>

### **Development Environment**
- Simplicity Studio IDE (SV5.10.2.0 version) and Debugger Integration. Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details
- Recommended to install and use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.12.1
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-ncp-manufacturing-utility-user-guide" for more details
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)
### **SDK**
- Simplified and Unified DX for Wi-Fi API 
- Simplifies application development and presents clean and standardized APIs
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub
### **Multi-protocol**
- Wi-Fi STA + BLE
### **PTA CoExistence**
- 3 wire CoEx acting as Wi-Fi with external Zigbee/OT
- 3 wire CoEx acting as Wi-Fi with external Bluetooth 
## **Changes in this Release Compared to v3.4.0 Release**
### **System**
- **Enhancements / New features** 
  - None
- **Fixed Issues** 
  - None
- **Documentation** 
  - None
### **SDK**
- **Enhancements / New features** 
  - Added compatibility support of si91x driver with Micrium OS. 
  - Renamed cli\_demo example to wireless\_test.
  - Added support of SiWN917Y100LGNBx(ACX) module auto detection feature to disable the region. 
  - Added support for XTAL and PMU good time configuration from host. 
- **Fixed Issues** 
  - Fixed usage of a combination of BSD and SiWx917 socket APIs on a single socket within the throughput application.
  - Fixed socket close handling during station rejoin failure. 
  - Removed unused bits from device configuration in http\_server application.
  - Fixed flushing of queues when asynchronous join or asynchronous IPV4 operation fails.
  - Fixed application halt when a 3rd party client connects to  SiWx917 AP and accesses the HTTP server in a loop. 
  - Fixed issue where DUT is unable to receive embedded MQTT data from 2nd iteration during MQTT connect and disconnect.
- **Documentation** 
  - Updated MQTT default ceritificate index uage in note section of sl\_mqtt\_tls\_flag\_t.
  - Updated all instances of SI917 to SiWx91x to maintain consistency in documentation. 
  - Updated option\_name documentation in setsockopt() API.
  - Updated HTTP extended header format README of http\_otaf and http\_otaf\_twt example application.
  - Enhanced memory configuration documentation in custom feature bitmap and extended custom feature bitmap. 
  - Added note to initialize wireless before using NVM3 APIs.
### **Wi-Fi/Network Stack**
- **Enhancements / New features** 
  - Added extended custom feature bitmap support to enable NWP QSPI 80MHz clock.  
  - Implemented support for programming XTAL and PMU good time from host. 
  - Added  support for EAP-PEAPSAFE1 and EAP-PEAPSAFE2 from Host. 
- **Fixed Issues** 
  - Renewed the EAP certificates (wifiuser.pem) for enterprise certificate based authentication. The wifiuser.pem is from Wi-Fi Alliance validation package for EAP certificate based Wi-Fi connection. This certificate is for validation purposes only, this should not be used for commercial purpose. 
  - Direct Sequence Spread Spectrum - DSSS parameter set IE  is added in the probe request for the channel information of the station. 
  - Fixed issue for loading encrypted firmware onto the plain board.
  - Fixed  firmware upgrade failing with error 0x1dd04. 
  - Fixed Crypto AES sample app timeout error. 
  - Fixed will-topic length issue.
  - Stabilize server socket handling in firmware when shutdown, remote terminate occur simultaneously.
- **Documentation** 
  - Updated the documentation on rejoin parameter. 
  - Updated the SSL error codes documentation. 
### **BLE**
- **Enhancements / New features** 
  - None
- **Fixed Issues** 
  - Fixed the issue where an incorrect MTU response was given if MTU exchange was first initiated by the Peer  
  - Fixed the BLE Indication API failure issue when enabling and disabling indications. 
  - Fixed the issue where BLE TX notifications stopped within 5 seconds during BLE TX/RX notifications
  - Fixed the BLE start advertising API failure while running BLE continuous direct connectable advertising 
  - Fixed the DUT is not throwing MTU response timeout error even after 30 seconds. 
  - Fixed DUT failed to disconnect or report any errors after SMP failure when passkey confirmation was unsuccessful
  - Fixed AE hang issue while running TX notifications in peripheral role.
- **Documentation** 
  - Updated the readme file for configuring the 2Mbps using the BLE\_2MBPS macro 
  - Updated the new error code (0x4046) for SL\_STATUS\_SI91X\_ERR\_BT\_INVALID\_ARGS when user provided invalid arguments 
### **Multi-protocol**
- **Enhancements / New features** 
  - None
- **Fixed Issues** 
  - None
- **Documentation** 
  - None
## **Recommendations**
### **System**
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP
- Memory configuration for NCP mode is 672K\_M4SS\_0K
- Set the following recommended FreeRTOS configuration in FreeRTOSConfig.h
  - configTIMER\_TASK\_PRIORITY to 55 
  - configTOTAL\_HEAP\_SIZE to 51200
  - configUSE\_POSIX\_ERRNO to 1
### **Wi-Fi/Network Stack**
- It is recommended to enable SL\_SI91X\_EXT\_TCP\_IP\_WAIT\_FOR\_SOCKET\_CLOSE BIT(16) of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer
- For high throughputs,  it is recommended to enable BIT(2) - SL\_SI91X\_FEAT\_AGGREGATION of feature\_bit\_map in opermode. 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative
  - In case of TWT in CoEx mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
  - DUT keepalive should be configured aligned with AP keepalive in TWT modes.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly
- The minimum keep alive interval  value recommended for embedded MQTT is 10 Seconds
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF
- Randomize the client port if using rapid socket connect/disconnect . 
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published while using Embedded MQTT, else it leads to return wrong error code(0x21).
- In concurrent mode with dual IP, it is advised to bring up STA first (IP configuration) and AP later
- It is recommended to configure Tx, Rx, Global buffer pool ratio in the buffer config command for all Wi-Fi Socket operations from the host
- It is recommended to use "TCP exponential backoff" configuration for congested channels
- It is recommended is to disable broadcast filter during TCP connection to avoid ARP resolution issues
- To avoid IOP issues, it is recommended to disable power save before Wi-Fi connection
- Enable BIT(10)  SL\_SI91X\_FEAT\_SSL\_HIGH\_STREAMING\_BIT in feature bitmap to increase TLS\_Rx throughputs.
- The ICMP timeout should be set to more than 2 seconds when power save is enabled. In power save mode, packet drops are expected when the timeout is low.
- An intermediate CA should be a trusted CA, provided it is not a self-signed certificate. To be considered as a trusted CA, the KeyUsage field must be explicitly configured while generating the certificate. 
### **BLE**
- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again
### **Multi-protocol**
- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout
  - Ensure Wi-Fi activity consumes lower intervals
## **Known Issues of WiSeConnect3\_SDK\_3.4.1 Release**
### **System**
- None
### **SDK**
- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number
- Matter extension based applications are experiencing compatibility issues with WiseConnect SDK 3.2.0. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s)
- Asynchronous Azure MQTT is not supported, this will be addressed in up coming release(s)
- mDNS with IPV6 is not supported
- Power Save with TCP/IP is not supported for UART interface
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer
- Recommended to use valid length(<= 202 bytes) for topic to be published in the "Embedded MQTT" demo, else it leads to return wrong error code(0x21)
- Observed AWS\_DEVICE SHADOW LOGGING STATS is not working with Power Save enable
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow
- Low Power examples usage and documentation still under scope of improvement.
- In WPA3 transition security mode, observed sl\_wifi\_get\_wireless\_info() API is giving wrong security type and PSK in both client and AP mode
- Observed sl\_wifi\_get\_operational\_statistics() API is giving wrong opermode and wrong beacon details in AP mode
- Observed MQTT Rx is not able to resume after rejoin in wifi\_station\_ble\_provisioning\_aws demo
- In Power save, user needs to increase application stack size for the Wi-Fi - AWS Device Shadow demo
- Observed DUT unable to wake up from Wi-Fi powersave deep sleep in case of without RAM retention over NCP UART
- Observed issue with the firmware update over the TCP in dense environment
- Observed socket close is not working as expected for TLS socket when socket connect, send data and socket close are performing in a continuous loop
- Observed UDP sendto API failure in wifi\_concurrent\_http\_server\_provisioning\_ncp application
- Observed connection issue with 3rd party AP In concurrent mode using webpage
- WMM-PS/UAPSD is not supported
- The PTA feature isn't working in NCP mode because the PTA pin configuration is conflicting with the NCP SPI/UART pin configuration. 
- Observed concurrent\_http\_server\_provisioning example readme is not updated as per application provided
### **Wi-Fi/Network Stack**
**Wi-Fi STA**

- STA connection with the WPA3 using the Hunting and Pecking algorithm takes approximately 3-4 seconds.
- Connection failures have been observed with certain APs in environments with high channel congestion (~50-60% occupancy in an open lab).
- Region selection based on country IE in the beacon is not supported for ICs.
- Intermittent beacon reception from Access Point (beacon misses) occurs when channel congestion exceeds 85%.
- Uplink MU-MIMO is not supported
- When scanning with low power mode enabled, a sensitivity degradation of 3-6dB is observed, which may prevent APs at longer ranges from appearing in the scan results.
- For ICs, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported
- For modules, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported in PER mode.
- Observed ~2% increase in listen current and ~1% increase in standby associated current.
- Tx max powers for EVM limited data rates (like MCS7, MCS6, 54M, etc) will be reduced by 0.5dB.
- NT profile connection is failing with AP not found(0x0003) error when enabled hidden mode in AP
- NT profile SSID's are not showing in scan results when we call scan API with extended scan results bit enabled

**Access Point (AP) Mode**

- Advance scan feature is not supported in AP mode. 

**WPA2 Enterprise security (STA)**

- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789"

**Wi-Fi Concurrency ( AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled Low density parity check coding

**MU-MIMO (DL)**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test
- Observed Performance, Interop issues with MU MIMO with certain APs
- Less throughput was observed in MU-MIMO with some APs that enabled Low density parity check coding

**MU-MIMO (UL)**

- UL MU-MIMO is not supported

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- Observed that the post rejoin, DUT is unable to publish the data to the cloud with the AWS MQTT application

**IPv4/IPv6**

- concurrent\_firmware\_update\_from\_host\_uart example have stability issues during rejoin or disconnection process

**BSD Socket API**

- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt
- Secure SSL renegotiation not supported in Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads

**SNTP**

- Unable to get SNTP async events when CoEx mode and power save are enabled 

**Embedded MQTT**

- Observed MQTT over SSL connection is failing on encrypted firmware. 

**Throughputs & Performance**

- Observed 20% less Wi-Fi throughput with SDK 3.x compare to target throughput (depends on host and host interface).

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations
- Observed OTA failures with power save enabled, So recommended to disable power save during OTA

**Wi-Fi IOT Cloud integration**

- **AWS IOT Core** 
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
  - Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with Powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency  is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 security
### **BLE**
**GAP**

- SPI interrupt miss issue was observed that prevents the host from receiving packets delivered by the NWP, when there is continuous BLE TX/RX data transfer using the ble\_multiconnection\_gatt\_test application

**Performance**

- The DUT hangs when the SRRC region is set in the ICs. However, this issue does not occur with the SiWN917Y module.

**Multi-protocol**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled
- While executing Wi-Fi Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application
- Observed BLE disconnection during wifi commissioning with few android mobiles
- Observed data stall on the remote machine during TCP/UDP transmission with power save enabled in  wifi\_throughput\_ble\_dual\_role\_ncp example
- For wifi\_ble\_powersave\_coex application, with 352K memory, observed 0xff2c - Memory limit exceeded in the given operating mode error.
- Observed throughput is not displaying for every interval of 'TEST\_TIMEOUT' when CONTINUOUS\_THROUGHPUT enabled for wifi\_station\_ble\_throughput\_app example.
### **PTA CoExistence**
- The Request and Priority pins in PTA are conflicting with the 
  - VCOM (EUSART0) pins in NCP SPI mode. 
  - NCP UART Hardware flow control pins in NCP UART mode
- VCOM(EUSART0)/NCP UART pin configuration should be changed.  
### **Simplicity Studio and Commander (For EFR Host)**
- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain
- Universal Configurator (UC) for EFR32xG Products in NCP Mode is not supported
## **Limitations and Unsupported Features**
### **System**
- 1.8v Supply is not supported
### **SDK**
- Baremetal mode is not supported
- WiSeConnect3\_SDK\_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade
- Lite Wireless firmware image is not supported for NCP mode.
- Zephyr is not supported
### **Wi-Fi/Network Stack**
- TLS 1.3 Server is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- A maximum of 3 SSL connections are supported in Wi-Fi alone and CoEx modes. No.of  SSL Sockets in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
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
- bTWT , Intra PPDU Power save, Spatial Re-Use, BSS coloring features not supported
- HTTPS server is not supported.
- In Wi-Fi Transceiver mode, MAC level encryption/decryption is not supported. 
- Depending on the crypto engine in use, except HMAC and SHA/SHA3, the input message length limit must not exceed its defined maximum. Refer to the crypto engine's documentation or SDK specifications for exact limits and details. 
- For WPA3 Enterprise connection in station mode,  
  - EAP-192 bit mode is not supported
  - UOSC feature is not supported
  - PEAPV1 is not supported
  - PMK caching is not supported.
  - Explicitly configured server certificate, server domain(FQDN), server domain suffix configuration are not supported for Server Certificate Validation.
- Customers shall not use sl\_si91x\_calibration\_write and sl\_si91x\_evm\_write APIs in PER mode when using module OPNs / Boards / Explorer Kits. This can lead to malfunctioning of the module.
### **BLE**
- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices
- BLE Slave latency value is valid up to 32 only
- BLE TX/RX throughput is less when tested with EFM as compared to EFR
- Maximum supported AE data length is 200 bytes
- Supports only two ADV\_EXT sets
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE
- Advertising Extension feature is not supported in Coexistence
- Isochronous channels feature is not supported
- Connection subrating feature is not supported
- LE power controller feature is not supported
- EATT feature is not supported
- Periodic Advertising with response(PAwR) feature is not supported
- BLE Audio is not supported
- The feature of dynamically changing the TX power when extended advertising is active is not supported
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address
- The maximum BLE power has been reduced by 2dB compared to the Datasheet Number, which will be addressed in the upcoming 3.3.1 patch release
- ICs do not support DEFAULT\_REGION and IGNORE\_REGION region codes.
- Modules do not support DEFAULT\_REGION and IGNORE\_REGION region codes in PER mode.
### **Multi-protocol**
- Wi-Fi AP + BLE currently not supported

> **Note:** The following BLE Synchronous API's will be deprecated soon and the equivalent Asynchronous API's will be used instead in all BLE applications : 
> 
> |**S.No**|**BLE Synchronous API's** |**BLE Asynchronous API's** |
> | :-: | :-: | :-: |
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

# **WiSeConnect3\_SDK\_3.4.0 NCP Release Notes**   

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|18th December 2024|
|SDK Version|3\.4.0|
|Firmware Version|Standard: 1711.2.13.3.0.0.11|
|GSDK/SiSDK Version|SiSDK 2024.12.0|
|Studio Version|5\.10.0.0|
|Release Package Name|WiSeConnect3\_SDK\_3.4.0|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components
  - Standard Wireless Firmware -  SiWx917 Firmware Binary available as SiWG917-B.2.13.3.0.0.11.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode.

Note:

- The release packages will have bug-fixes, enhancements, and new features in both 'SDK' and 'Firmware'. Customer shall update and use 'SDK' and 'Firmware' of same release package. SDK and FW combinations that are not released together are not supported.

## **Supported Hardware OPNs**

|**Hardware**|**OPN (Ordering Part Number)**|
| :- | :- |
|IC OPN|<p>QFN OPN: SiWN917M100LGTBA (Wi-Fi 6 NCP IC, QFN 7x7, 2.4 GHz, 4MB stacked flash, -40 to +85C​) </p><p>Module OPN: SiWN917Y100LGNBx</p>|
|Expansion kits:|<p>SiWx917-EB4346A (based on Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)</p><p>Module Board: SiW917Y-RB4357A (SiWN917Y Module Wi-Fi 6 and Bluetooth LE 4MB Flash RF-Pin Co-Processor Radio Board)</p>|

## **Supported Features** 

<table><tr><th><b>Section</b></th><th><b>Sub-Section</b></th><th><b>Feature</b></th></tr>
<tr><td rowspan="9">System</td><td rowspan="5">Operating modes</td><td>Wi-Fi STA (802.11ax, 802.11n)</td></tr>
<tr><td>Wi-Fi 802.11n AP</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + BLE</td></tr>
<tr><td valign="top">Wi-Fi Transceiver (802.11 b/g)</td></tr>
<tr><td>Security </td><td>Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Debug Lock, Flash Protection</td></tr>
<tr><td>Secure firmware upgrade options</td><td><p>- Firmware loading through UART, SPI Interface</p><p>- Secure Over the Air (OTA) Upgrade</p><p>- Firmware update via Bootloader</p></td></tr>
<tr><td>Crypto Support</td><td><p>- Crypto API's for Hardware Accelerators: Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)</p><p>- Software Accelerators: RSA, ECC</p><p>- Wrapping Secret keys (Symmetric crypto). </p><p>- Added ECDSA Sign and Verify APIs</p></td></tr>
<tr><td>System Power Save</td><td><p>- Deep Sleep with RAM retention and without RAM retention. </p><p>- Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. </p></td></tr>
<tr><td rowspan="13">Wi-Fi</td><td>Wi-Fi Protocols</td><td>IEEE 802.11 b/g/n/ax (2.4GHz)</td></tr>
<tr><td>Access Point (AP) Mode</td><td><p>- 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)</p><p>- Wi-Fi Security </p><p>- WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2) </p></td></tr>
<tr><td>Wi-Fi Scan</td><td>Selective Scan, Active/Passive Scan</td></tr>
<tr><td>Wi-Fi STA (Security Modes)</td><td>Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal,  WPA3 Enterprise, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)</td></tr>
<tr><td>WPA2 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td></tr>
<tr><td>WPA3 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td></tr>
<tr><td>Wi-Fi STA Rejoin</td><td></td></tr>
<tr><td>Wi-Fi STA Roaming </td><td>BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication</td></tr>
<tr><td>Wi-Fi Protocol Power Save </td><td>Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP, TWT</td></tr>
<tr><td>QoS</td><td>WMM-QoS</td></tr>
<tr><td>Wi-Fi 6 Feature</td><td>MU-MIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID</td></tr>
<tr><td>Wi-Fi Concurrency </td><td>AP+STA (Same channel)</td></tr>
<tr><td>Wi-Fi Band/Channels</td><td>2.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14</td></tr>
<tr><td>Known Security Vulnerabilities Handled</td><td>WPA2 KRACK Attacks, Fragment and Forge Vulnerability</td></tr>
<tr><td rowspan="4">Network stack</td><td>Core Networking Features</td><td><p>- IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6</p><p>- SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 </p><p>- SSL server versions TLSV1.0 and TLSV1.2</p><p>- DHCPv4 Client,DHCPv6 Client</p><p>- DHCPv4 Server,DHCPv6 Server</p><p>- TCP/IP Bypass (LWIP as Hosted stack for reference)</p></td></tr>
<tr><td>Advanced Network Features</td><td>- HTTP Client/HTTPS Client/DNS Client/SNTP Client, IGMP, Embedded MQTT, MQTT on host (AWS and AZURE)</td></tr>
<tr><td>Wi-Fi IoT Cloud Integration</td><td><p>- AWS IoT Core</p><p>- Azure IoT Core</p></td></tr>
<tr><td>BSD and IoT sockets application programming interface(API)</td><td></td></tr>
<tr><td rowspan="2">BLE </td><td>Legacy features</td><td><p>- GAP(Advertising, Scanning, initiation, Connection and Bonding)</p><p>- Generic Attribute Protocol(GATT)</p><p>- Attribute protocol(ATT)</p><p>- Security</p><p>- LL Privacy 1.2</p><p>- Accept list</p><p>- Directed Advertising</p><p>- LE PHY(1Mbps, 2Mbps) & Coded PHY(125kbps, 500kbps)</p><p>- Simultaneous scanning on 1Mbps and Coded PHY</p><p>- LE dual role topology</p><p>- LE data packet length extensions(DLE)</p><p>- Asymmetric PHYs</p><p>- LE channel selection algorithm 2 (CSA#2)</p><p>- LE Secure connections</p><p>- Bluetooth 5.4 Qualified</p></td></tr>
<tr><td>Advertising Extensions </td><td><p>- Extended Advertising</p><p>- Periodic Advertising</p><p>- Periodic Advertising scanning</p><p>- Extended Advertising scanning</p><p>- Periodic Advertising list</p><p>- LE periodic advertising synchronization</p></td></tr>
</table>

### **Development Environment**

- Simplicity Studio IDE (SV5.10.0.0 version) and Debugger Integration. Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details
- Recommended to install and use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.12.0
- Simplicity Commander to support Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. Refer "siwx917-ncp-manufacturing-utility-user-guide" for more details
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)

### **SDK**

- Simplified and Unified DX for Wi-Fi API 
- Simplifies application development and presents clean and standardized APIs
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

**PTA CoExistence**

- 3 wire CoEx acting as Wi-Fi with external Bluetooth 
- 3 wire CoEx acting as Wi-Fi with external Zigbee/OT


## **Changes in this release compared to v3.3.4 Release**

### **System**

- **Enhancements / New features**
  - Firmware Update error codes have been revised, introducing new error code while modifying existing ones. Users who rely on numeric status codes (e.g., 0x00) in their applications must update their implementations accordingly. For detailed guidance on these changes, please refer the migration guide to ensure smooth adaptation to the new error code structure. 
- **Fixed Issues**
  - None
- **Documentation**
  - None

### **SDK**

- **Enhancements / New features**
  - Enhanced internal bus to handle multiple socket receive requests.  
  - Added new example named "Wi-Fi - Multithread Sockets (NCP)".
  - Added a new example named "Wi-Fi - Provisioning via Access Point (NCP)".  
  - Added option to configure Azure MQTT Keepalive timeout in preprocessor settings. 
  - Added WPA3 security option on the login page for the concurrent\_http\_server\_provisioning application. 
  - Added support to configure default region configuration for AP and Concurrent mode. 
  - Updated socket option value TOS to use standard macros for both offload and hosted mode.  
  - Moved powersave\_standby\_associated application into low power example folder.  
  - Added option to configure the event handler thread stack size using SL\_SI91X\_EVENT\_HANDLER\_STACK\_SIZE.  
  - Updated enterprise\_client example with DHCP configuration. 
  - Removed the dependency of the network manager component from the Wi-Fi component. 
  - Added support for Amazon root CA1 certificate to AWS applications. 
  - Updated http\_otaf example to use multiple certificate indexes.  
  - Added support for handling remote termination in data\_transfer example.  
  - Added command support to configure 11ax, select region and BLE in CLI Demo
  - Added SL\_SI91X\_EXT\_FEAT\_DISABLE\_XTAL\_CORRECTION bit to disable the automatic correction of the 40MHz crystal. This bit must be enabled during calibration or when not using the recommended crystal part
  - Added new API sl\_si91x\_get\_nwp\_config added to get OPN part number 
  - Renamed sl\_net\_dns\_get\_by\_hostname() to sl\_net\_dns\_resolve\_hostname() to user-friendly name.  
  - Added support for sl\_si91x\_debug\_log() API. 
- **Fixed Issues**
  - Fixed internal memory deallocation when rejoining in aws\_mqtt application. 
  - Fixed precondition of calling sl\_wifi\_init() before calling sl\_wifi\_set\_advanced\_client\_configuration() API.  
  - Fixed data writing speed to flash firmware using firmware\_flashing\_from\_host\_uart\_xmodem application.  
  - Fixed junk values when background scan is enabled. 
  - Fixed the issue where the parameter sl\_wifi\_advanced\_scan\_configuration\_t.enable\_instant\_scan was hardcoded to ENABLE\_INSTANT\_SCAN. Now, its value is dynamically taken as input from the user.  
  - Fixed memory alignment in sl\_wifi\_buffer\_t structure.  
  - Fixed Wi-Fi scan result shared with DUT terminal vs SiLabs apps on 3rd party device while using wifi\_station\_ble\_provisioning and wifi\_station\_ble\_provisiong\_aws applications. 
  - Fixed BSD error EBADF when sending data in the concurrent HTTP server example.  
  - Fixed invalid credential ID checks for sl\_net\_get\_credential() API  
  - Fixed remote disconnection error while keeping the idle connection of Azure MQTT.  
  - Fixed the inability to change LI configuration without changing the performance profile.  

  - Fixed invalid command error for sl\_calib\_write command using the Wi-Fi calibration application.
  - Fixed issue in setting max listen interval for AP mode while using  sl\_wifi\_set\_listen\_interval() API.  
  - Fixed an issue with PSK and SSID to use generic phrases in the CLI demo application, and reflected this change in the README file.  
  - Fixed an issue where the DUT failed to enter sleep mode in the standby power save application.  
  - Fixed an issue in the wifi\_power\_save\_deep\_sleep example, which encountered error 0x11 when the enable\_high\_performance() API was used. 
  - Fixed TCP/UDP transmission failures due to error 55 (No Buffers Available) in the multithread\_sockets application.
  - Fixed processing of space in HTTP header in sl\_http\_server\_get\_request\_headers() API. 
  - Fixed issue where DUT failed with BBE2 error (HTTP Failed) during HTTP GET/POST operations when DUT acts as both HTTP server and HTTP client.  
  - Fixed power save modes terminology as per the datasheet. 
  - Fixed thread-local array ID overwrites when sl\_si91x\_wireless and sl\_iostream components were used together. 
  - Fixed socket close handling during Wi-Fi disconnects or rejoin failures. 
  - Fixed interface up failures in sl\_wifi\_set\_max\_tx\_power() API for AP mode.  
  - Fixed timeout error when secured combined Image is flashed through http\_otaf example.
  - Fixed memory allocation failures in concurrent\_http\_server\_provisioning example.  
  - Fixed HTTP OTAF firmware upgrade failure with combined AWS certificate.  
  - Fixed si91x socket typedefs signature to align with standard naming conventions.  
  - Fixed issue where DUT is unable to receive more than 800 bytes of data from azure cloud.  
  - Fixed issue where aws\_mqtt application gets struck when running in a loop.  
  - Fixed handling of flushing disconnected client socket connection in AP mode. 
  - Fixed issue in setting data rate using sl\_wifi\_set\_transmit\_rate API in AP mode.  
  - Fixed a timeout error that occurred when APUT was disconnecting a connected third-party client.  
  - sl\_si91x\_setsockopt\_async has been renamed to sl\_si91x\_setsockopt. sl\_si91x\_set\_custom\_sync\_sockopt() and sl\_si91x\_get\_custom\_sync\_sockopt() are replaced by setsockopt and getsockopt respectively. 
  - Fixed compilation issue for STM32 in Keil IDE with user gain table example 
- **Documentation**
  - Updated the notes section regarding the certificate format to be used in the sl\_net\_set\_credential()  API. 
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
  - Updated note section of sl\_si91x\_setsockopt\_async()and sl\_si91x\_set\_custom\_sync\_sockopt() with limitation of API for sockets. 
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
  - Added readme for multi thread sockets example 
  - Updated the WebSocket API documentation 

### **Wi-Fi/Network Stack**

- **Enhancements / New features**
  - Enhanced support for alternate certification chains for TLS has been implemented. With this update, only the peer certificate needs to validate against a trusted certificate, eliminating the need for full chain validation. This improvement allows the use of intermediate root CAs as trusted entities, enhancing flexibility in certificate management.
  - Added support to configure DHCP retry parameters (maximum retry count and retry interval) from the SDK
  - Added support for HMAC wrapped keys 
  - Added WPA3 Enterprise support in STA 
  - Added support for IPv6 address change notification.  
  - Added support to handle 20 total number of sockets using sl\_si91x\_select() API. 
  - Added support for sl\_net\_configure\_ip() and sl\_net\_get\_ip\_address() APIs. 
  - Added thread-safe support for socket select API in BSD, IoT and Si91x sockets. 
  - Added support for WebSocket client feature and example.  

- **Fixed Issues**
  - Fixed issues in DNS when WLAN disconnect is issued from SDK.
  - The TCP maximum retry value can now be programmed to more than 31.
  - Fixed issues with the TCP retry in Power Save mode .
  - Fixed issues with TCP socket remote termination notifications during repeated socket open and close operations executed in a loop.
  - Fixed issue in concurrent mode where the STA's IPv6 configuration could fail in DHCP stateless mode if the STA initialized after the AP was already active.
  - Fixed an issue in concurrent mode where data transfer using a Link-local address always defaulted to the first IP interface created, regardless of the intended destination.
  - Fixed an issue where the STA in static IP configuration ignored the gateway address when connected to a third-party AP operating in stateless mode.
  - Stabilized the IPv6 DAD (Duplicate Address Detection) process during WLAN connection and disconnection sequences.
  - Resolved issues with socket data transfer in concurrent mode
  - Fixed an issue where DUT in concurrent - AP mode does not process the deauthentication frame from third party STA 
  - Fixed an issue where UDP traffic stopped when 3rd party station connects and disconnects to APUT 
  - Fixed the issue of asynchronous events in concurrent mode, when host issues disconnect to the 3rd party station connected to APUT. 
  - Fixed issue where APUT responds to the Probe Request with Direct Sequence Spread Spectrum (DSSS) IE containing a different channel than APUT operating channel 
  - Fixed issue with MQTT state mismatch in case of Wi-Fi disconnection. 
  - Fixed handling of closing server socket to close all associated client sockets.  
  - Fixed AP start failure (0xf) using SL\_WIFI\_WPA2 security and SL\_WIFI\_DEFAULT\_ENCRYPTION encryption.
  - Fixed issue where DUT is not initiating the authentication process and returned SL\_STATUS\_FAIL with PMK enabled.  
  - Fixed error in MSS calculation for TCP sockets in sl\_si91x\_get\_socket\_mss() API.  
  - Fixed issue where HTTP headers aren't having proper carriage and line feed at the end of the request.  
  - Fixed issue where the net up and down of the AP DUT interface was causing UDP transfers to stop for the station DUT in concurrent mode. 

- **Documentation**
  - Updated the HTTP long URL documentation.
  - Updated the documentation for ICMP packets with large sizes.

### **BLE**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - Resolved the Just Works issue by removing the transmission of the passkey event to the Host as per specification.
  - Resolved DUT hang issue while BLE advertising and scanning with power save and encryption enabled.
  - Resolved issue with BLE AE periodic sync cancel and sync terminate API flow in the ble\_ae\_central application.
  - Fixed random hopping issue in PER mode.
  - Fixed BLE indication and write with response data transfer issues with AE.
  - Resolved the issue where the DUT is not generating a passkey using the passkey entry method when attempting to connect with legacy paring. 
- **Documentation**
  - Added a note that we removed the BLE packet transmission on channel-39 (2480MHz) at a 2Mbps data rate as per Specification in the  rsi\_ble\_per\_transmit\_s structure 
  - Segregated BLE macros and callbacks to groups with appropriate description. 
  - Updated sl\_wifi\_init API details in BLE files.  
  - Added Zlib license clause for Zephyr integrated BLE files.

### **Multi-protocol**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - Resolved the BLE and WLAN connection failure with SMP when WLAN connect and HTTPS GET were called in a loop.
  - Resolved BLE bonding failure issue during continuous HTTPS download.
- **Documentation**
  - None

## **Recommendations**

### **System**

- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP
- Memory configuration for NCP mode is 672K\_M4SS\_0K
- Set the following recommended FreeRTOS configuration in FreeRTOSConfig.h
  - configTIMER\_TASK\_PRIORITY to 55 
  - configTOTAL\_HEAP\_SIZE to 51200
  - configUSE\_POSIX\_ERRNO to 1

### **Wi-Fi/Network Stack**

- It is recommended to enable SL\_SI91X\_EXT\_TCP\_IP\_WAIT\_FOR\_SOCKET\_CLOSE BIT(16) of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer
- For high throughputs,  it is recommended to enable BIT(2) - SL\_SI91X\_FEAT\_AGGREGATION  of feature\_bit\_map in opermode. 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative
  - In case of TWT in CoEx mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
  - DUT keepalive should be configured aligned with AP keepalive in TWT modes.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly
- The minimum keep alive interval  value recommended for embedded MQTT is 10 Seconds
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published while using Embedded MQTT, else it leads to return wrong error code(0x21).
- In concurrent mode with dual IP, it is advised to bring up STA first (IP configuration) and AP later
- It is recommended to configure Tx, Rx, Global buffer pool ratio in the buffer config command for all Wi-Fi Socket operations from the host
- It is recommended to use "TCP exponential backoff" configuration for congested channels
- It is recommended is to disable broadcast filter during TCP connection to avoid ARP resolution issues
- To avoid IOP issues, it is recommended to disable power save before Wi-Fi connection
- Enable BIT(10)  SL\_SI91X\_FEAT\_SSL\_HIGH\_STREAMING\_BIT in feature bitmap to increase TLS\_Rx throughputs.
- The ICMP timeout should be set to more than 2 seconds when power save is enabled. In power save mode, packet drops are expected when the timeout is low.
- An intermediate CA should be a trusted CA, provided it is not a self-signed certificate. To be considered as a trusted CA, the KeyUsage field must be explicitly configured while generating the certificate. 

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout
  - Ensure Wi-Fi activity consumes lower intervals

## **Known Issues of WiSeConnect3\_SDK\_3.4.0 Release**

### **System**

- None

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential. 
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number
- Matter extension based applications are experiencing compatibility issues with WiseConnect SDK 3.2.0. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s)
- Asynchronous Azure MQTT is not supported, this will be addressed in up coming release(s)
- mDNS with IPV6 is not supported
- Power Save with TCP/IP is not supported for UART interface
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer
- Recommended to use valid length(<= 202 bytes) for topic to be published in the "Embedded MQTT" demo, else it leads to return wrong error code(0x21)
- Observed AWS\_DEVICE SHADOW LOGGING STATS is not working with Power Save enable
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow
- Low Power examples usage and documentation still under scope of improvement.
- In WPA3 transition security mode, observed sl\_wifi\_get\_wireless\_info() API is giving wrong security type and PSK in both client and AP mode
- Observed sl\_wifi\_get\_operational\_statistics() API is giving wrong opermode and wrong beacon details in AP mode
- Observed MQTT Rx is not able to resume after rejoin in wifi\_station\_ble\_provisioning\_aws demo
- In Power save, user needs to increase application stack size for the Wi-Fi - AWS Device Shadow demo
- Observed DUT unable to wake up from Wi-Fi powersave deep sleep in case of without RAM retention over NCP UART
- Observed issue with the firmware update over the TCP in dense environment
- Observed socket close is not working as expected for TLS socket when socket connect, send data and socket close are performing in a continuous loop
- Observed UDP sendto API failure in wifi\_concurrent\_http\_server\_provisioning\_ncp application
- Observed connection issue with 3rd party AP In concurrent mode using webpage
- WMM-PS/UAPSD is not supported
- The PTA feature isn't working in NCP mode because the PTA pin configuration is conflicting with the NCP SPI/UART pin configuration. 
- Extended header is not visible in sniffer captures while running http otaf example 
- For SiWN917Y modules, in boot configurations, it is required to set the region_code to IGNORE_REGION except when in PER mode.
- Observed concurrent\_http\_server\_provisioning example readme is not updated as per application provided 

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA connection with the WPA3 using the Hunting and Pecking algorithm takes approximately 3-4 seconds.
- Connection failures have been observed with certain APs in environments with high channel congestion (~50-60% occupancy in an open lab).
- Region selection based on country IE in the beacon is not supported for ICs.
- Intermittent beacon reception from Access Point (beacon misses) occurs when channel congestion exceeds 85%.
- Uplink MU-MIMO is not supported
- When scanning with low power mode enabled, a sensitivity degradation of 3-6dB is observed, which may prevent APs at longer ranges from appearing in the scan results.
- For ICs, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported
- For modules, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported in PER mode.
- Observed ~2% increase in listen current and ~1% increase in standby associated current.
- Tx max powers for EVM limited data rates (like MCS7, MCS6, 54M, etc) will be reduced by 0.5dB.

**Access Point (AP) Mode**
 
- Advance scan feature is not supported in AP mode. 

**WPA2 Enterprise security (STA)**

- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789"

**Wi-Fi Concurrency ( AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios
- In concurrent mode, if IP is configured for AP mode ahead of STA mode then IPv6 configuration may fail for STA mode
- In concurrent mode, data transfer using the Link-local address will always use the first IP interface created by the application.
- In concurrent mode, 917 AP cannot process de-authentication frames sent by third-party STA if 917 STA is connected to WPA2+WPA3 enabled AP .

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled Low density parity check coding

**MU-MIMO (DL)**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test
- Observed Performance, Interop issues with MU MIMO with certain APs
- Less throughput was observed in MU-MIMO with some APs that enabled Low density parity check coding

**MU-MIMO (UL)**

- UL MU-MIMO is not supported

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- Observed that the post rejoin, DUT is unable to publish the data to the cloud with the AWS MQTT application

**IPv4/IPv6**

- concurrent\_firmware\_update\_from\_host\_uart example have stability issues during rejoin or disconnection process

**BSD Socket API**

- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt
- Secure SSL renegotiation not supported in Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads

**SNTP**

- Unable to get SNTP async events when CoEx mode and power save are enabled 

**Throughputs & Performance**

- Observed 20% less Wi-Fi throughput with SDK 3.x compare to target throughput (depends on host and host interface), SDK refinements are in progress

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations
- Observed OTA failures with power save enabled, So recommended to disable power save during OTA

**Wi-Fi IOT Cloud integration**

- **AWS IOT Core**
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
  - Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with Powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency  is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 security

### **BLE**

**GAP**

- SPI interrupt miss issue was observed that prevents the host from receiving packets delivered by the TA, when there is continuous BLE TX/RX data transfer using the ble\_multiconnection\_gatt\_test application
- The DUT is unable to move to active state after BLE link loss using SiWN917Y100LGNB4 OPN
- BLE start advertising API failed with error code 0xffffffe2 while sending BLE direct connectable advertising packet continuously.
- DUT is not throwing MTU response timeout error after a timeout of 30 seconds.
- BLE indication API failed with error code 0xFFFFFFD9(ATT command in progress) while enabling and disabling indications continuously.

**AE** 

- Observed DUT hang issue while running TX notifications in peripheral role.

**Performance**

- The DUT hangs when the SRRC region is set in the ICs. However, this issue does not occur with the SiWN917Y module.

### **Multi-protocol**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled
- While executing Wi-Fi Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application
- Observed BLE disconnection during wifi commissioning with few android mobiles
- Observed data stall on the remote machine during TCP/UDP transmission with power save enabled in  wifi\_throughput\_ble\_dual\_role\_ncp example
- For wifi\_ble\_powersave\_coex application, with 352K memory, observed 0xff2c - Memory limit exceeded in the given operating mode error.
- Observed throughput is not displaying for every interval of 'TEST\_TIMEOUT' when CONTINUOUS\_THROUGHPUT enabled for wifi\_station\_ble\_throughput\_app example.

**PTA CoExistence**

- The Request and Priority pins in PTA are conflicting with the 

  - VCOM (EUSART0) pins in NCP SPI mode. 
  - NCP UART Hardware flow control pins in NCP UART mode

  VCOM(EUSART0)/NCP UART pin configuration should be changed.


### **Simplicity Studio and Commander (For EFR Host)**

- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain
- Universal Configurator (UC) for EFR32xG Products in NCP Mode is not supported

## **Limitations and Unsupported Features**

### **System**

- 1.8v Supply is not supported

### **SDK**

- Baremetal mode is not supported
- WiSeConnect3\_SDK\_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade
- Lite Wireless firmware image is not supported for NCP mode.
- Zephyr is not supported

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- A maximum of 3 SSL connections are supported in Wi-Fi alone and CoEx modes. No.of  SSL Sockets in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
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
- bTWT , Intra PPDU Power save, Spatial Re-Use, BSS coloring features not supported
- HTTPS server is not supported.
- In Wi-Fi Transceiver mode, MAC level encryption/decryption is not supported. 
- Depending on the crypto engine in use, except HMAC and SHA/SHA3, the input message length limit must not exceed its defined maximum. Refer to the crypto engine's documentation or SDK specifications for exact limits and details. 
- For WPA3 Enterprise connection in station mode, 
  - EAP-192 bit mode is not supported
  - UOSC feature is not supported
  - PEAPV1 is not supported
  - PMK caching is not supported.
  - Explicitly configured server certificate, server domain(FQDN), server domain suffix configuration are not supported for Server Certificate Validation.
- NT profile connection is failing with AP not found(0x0003) error when enabled hidden mode in AP. 
- NT profile SSID's are not showing in scan results when we call scan API with extended scan results bit enabled.
- Customers shall not use sl\_si91x\_calibration\_write and sl\_si91x\_evm\_write APIs in PER mode when using module OPNs / Boards / Explorer Kits. This can lead to malfunctioning of the module. 

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices
- BLE Slave latency value is valid up to 32 only
- BLE TX/RX throughput is less when tested with EFM as compared to EFR
- Maximum supported AE data length is 200 bytes
- Supports only two ADV\_EXT sets
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE
- Advertising Extension feature is not supported in Coexistence
- Isochronous channels feature is not supported
- Connection subrating feature is not supported
- LE power controller feature is not supported
- EATT feature is not supported
- Periodic Advertising with response(PAwR) feature is not supported
- BLE Audio is not supported
- The feature of dynamically changing the TX power when extended advertising is active is not supported
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address
- The maximum BLE power has been reduced by 2dB compared to the Datasheet Number, which will be addressed in the upcoming 3.3.1 patch release
- ICs do not support DEFAULT\_REGION and IGNORE\_REGION region codes.
- Modules do not support DEFAULT\_REGION and IGNORE\_REGION region codes in PER mode.

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported

> **Note:** 
>
> The following BLE Synchronous API's will be deprecated soon and the equivalent Asynchronous API's will be used instead in all BLE applications :
> 
> |**S.No**|**BLE Synchronous API's** |**BLE Asynchronous API's** |
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

# **WiSeConnect3\_SDK\_3.3.4 NCP Release Notes**   

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|18th October 2024|
|SDK Version|3\.3.4|
|Firmware Version|Standard: 1711.2.12.3.3.0.3|
|GSDK/SiSDK Version|SiSDK 2024.6.2 |
|Studio Version|5\.9.3.0|
|Release Package Name|WiSeConnect3\_SDK\_3.3.4|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components
  - Standard Wireless Firmware -  SiWx917 Firmware Binary available as SiWG917-B.2.12.3.3.0.3.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode.

Note:

- The release packages will have bug-fixes, enhancements, and new features in both 'SDK' and 'Firmware'. Customer shall update and use 'SDK' and 'Firmware' of same release package. SDK and FW combinations that are not released together are not supported.

## **Supported Hardware OPNs**

|**Hardware**|**OPN (Ordering Part Number)**|
| :- | :- |
|IC OPN|<p>QFN OPN: SiWN917M100LGTBA (Wi-Fi 6 NCP IC, QFN 7x7, 2.4 GHz, 4MB stacked flash, -40 to +85C​) </p><p>Module OPN: SiWN917Y100LGNBx</p>|
|Expansion kits:|<p>SiWx917-EB4346A (based on Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)</p><p>Module Board: SiW917Y-RB4357A (SiWN917Y Module Wi-Fi 6 and Bluetooth LE 4MB Flash RF-Pin Co-Processor Radio Board)</p>|

## **Supported Features** 

<table><tr><th><b>Section</b></th><th><b>Sub-Section</b></th><th><b>Feature</b></th></tr>
<tr><td rowspan="9">System</td><td rowspan="5">Operating modes</td><td>Wi-Fi STA (802.11ax, 802.11n)</td></tr>
<tr><td>Wi-Fi 802.11n AP</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + BLE</td></tr>
<tr><td>Wi-Fi Transceiver (802.11 b/g)</td></tr>
<tr><td>Security </td><td>Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Debug Lock, Flash Protection</td></tr>
<tr><td>Secure firmware upgrade options</td><td><p>- Firmware loading through UART, SPI Interface</p><p>- Secure Over the Air (OTA) Upgrade</p><p>- Firmware update via Bootloader</p></td></tr>
<tr><td>Crypto Support</td><td><p>- Crypto API's for Hardware Accelerators: Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)</p><p>- Software Accelerators: RSA, ECC</p><p>- Wrapping Secret keys (Symmetric crypto). </p><p>- Added ECDSA Sign and Verify APIs</p></td></tr>
<tr><td>System Power Save</td><td><p>- Deep Sleep with RAM retention and without RAM retention. </p><p>- Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. </p></td></tr>
<tr><td rowspan="13">Wi-Fi</td><td>Wi-Fi Protocols</td><td>IEEE 802.11 b/g/n/ax (2.4GHz)</td></tr>
<tr><td>Access Point (AP) Mode</td><td><p>- 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)</p><p>- Wi-Fi Security </p><p>- WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2) </p></td></tr>
<tr><td>Wi-Fi Scan</td><td>Selective Scan, Active/Passive Scan</td></tr>
<tr><td>Wi-Fi STA (Security Modes)</td><td>Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)</td></tr>
<tr><td>WPA2 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td></tr>
<tr><td>Wi-Fi STA Rejoin</td><td></td></tr>
<tr><td>Wi-Fi STA Roaming </td><td>BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication</td></tr>
<tr><td>Wi-Fi Protocol Power Save </td><td>Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP, TWT</td></tr>
<tr><td>QoS</td><td>WMM-QoS</td></tr>
<tr><td>Wi-Fi 6 Feature</td><td>MU-MIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID</td></tr>
<tr><td>Wi-Fi Concurrency </td><td>AP+STA (Same channel)</td></tr>
<tr><td>Wi-Fi Band/Channels</td><td>2.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14</td></tr>
<tr><td>Known Security Vulnerabilities Handled</td><td>WPA2 KRACK Attacks, Fragment and Forge Vulnerability</td></tr>
<tr><td rowspan="4">Network stack</td><td>Core Networking Features</td><td><p>- IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6</p><p>- SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 </p><p>- SSL server versions TLSV1.0 and TLSV1.2</p><p>- DHCPv4 Client,DHCPv6 Client</p><p>- DHCPv4 Server,DHCPv6 Server</p><p>- TCP/IP Bypass (LWIP as Hosted stack for reference)</p></td></tr>
<tr><td>Advanced Network Features</td><td>- HTTP Client/HTTPS Client/DNS Client/SNTP Client, Embedded MQTT, MQTT on host, IGMP</td></tr>
<tr><td>Wi-Fi IoT Cloud Integration</td><td><p>- AWS IoT Core</p><p>- Azure IoT Core</p></td></tr>
<tr><td>BSD and IoT sockets application programming interface(API)</td><td></td></tr>
<tr><td rowspan="2">BLE </td><td>Legacy features</td><td><p>- GAP(Advertising, Scanning, initiation, Connection and Bonding)</p><p>- Generic Attribute Protocol(GATT)</p><p>- Attribute protocol(ATT)</p><p>- Security</p><p>- LL Privacy 1.2</p><p>- Accept list</p><p>- Directed Advertising</p><p>- LE PHY(1Mbps, 2Mbps) & Coded PHY(125kbps, 500kbps)</p><p>- Simultaneous scanning on 1Mbps and Coded PHY</p><p>- LE dual role topology</p><p>- LE data packet length extensions(DLE)</p><p>- Asymmetric PHYs</p><p>- LE channel selection algorithm 2 (CSA#2)</p><p>- LE Secure connections</p><p>- Bluetooth 5.4 Qualified</p></td></tr>
<tr><td>Advertising Extensions </td><td><p>- Extended Advertising</p><p>- Periodic Advertising</p><p>- Periodic Advertising scanning</p><p>- Extended Advertising scanning</p><p>- Periodic Advertising list</p><p>- LE periodic advertising synchronization</p></td></tr>
</table>

### **Development Environment**

- Simplicity Studio IDE (SV5.9.3.0 version) and Debugger Integration. Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details
- Recommended to install and use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.6.2
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-ncp-manufacturing-utility-user-guide" for more details
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)

### **SDK**

- Simplified and Unified DX for Wi-Fi API 
- Simplifies application development and presents clean and standardized APIs
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA CoExistence**

- 3 wire CoEx acting as Wi-Fi with external Bluetooth 
- 3 wire CoEx acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.3.3 Release**

### **System**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - None
- **Documentation**
  - **None**

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

- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP
- Memory configuration for NCP mode is 672K\_M4SS\_0K
- Set the following recommended FreeRTOS configuration in FreeRTOSConfig.h
  - configTIMER\_TASK\_PRIORITY to 55 
  - configTOTAL\_HEAP\_SIZE to 51200
  - configUSE\_POSIX\_ERRNO to 1

### **Wi-Fi/Network Stack**

- It is recommended to enable SL\_SI91X\_EXT\_TCP\_IP\_WAIT\_FOR\_SOCKET\_CLOSE BIT(16) of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer
- For high throughputs,  it is recommended to enable BIT(2) - SL\_SI91X\_FEAT\_AGGREGATION  of feature\_bit\_map in opermode. 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative
  - In case of TWT in CoEx mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
  - DUT keepalive should be configured aligned with AP keepalive in TWT modes.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly
- The minimum keep alive interval  value recommended for embedded MQTT is 10 Seconds
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published while using Embedded MQTT, else it leads to return wrong error code(0x21).
- In concurrent mode with dual IP, it is advised to bring up STA first (IP configuration) and AP later
- It is recommended to configure Tx, Rx, Global buffer pool ratio in the buffer config command for all Wi-Fi Socket operations from the host
- It is recommended to use "TCP exponential backoff" configuration for congested channels
- It is recommended is to disable broadcast filter during TCP connection to avoid ARP resolution issues
- To avoid IOP issues, it is recommended to disable power save before Wi-Fi connection
- Enable BIT(10)  SL\_SI91X\_FEAT\_SSL\_HIGH\_STREAMING\_BIT in feature bitmap to increase TLS\_Rx throughputs.
- It is recommended to set region\_code as `IGNORE_REGION` in boot configurations for ACx module boards except for PER mode.

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout
  - Ensure Wi-Fi activity consumes lower intervals

## **Known Issues of WiSeConnect3\_SDK\_3.3.4 Release**

### **System**

- None

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number
- Matter extension based applications are experiencing compatibility issues with WiseConnect SDK 3.2.0. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s)
- Asynchronous Azure MQTT is not supported, this will be addressed in up coming release(s)
- mDNS with IPV6 is not supported
- Power Save with TCP/IP is not supported for UART interface
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer
- Recommended to use valid length(<= 202 bytes) for topic to be published in the "Embedded MQTT" demo, else it leads to return wrong error code(0x21)
- Observed AWS\_DEVICE SHADOW LOGGING STATS is not working with Power Save enable
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow
- Low Power examples usage and documentation still under scope of improvement.
- In WPA3 transition security mode, observed sl\_wifi\_get\_wireless\_info() API is giving wrong security type and PSK in both client and AP mode
- Observed sl\_wifi\_get\_operational\_statistics() API is giving wrong opermode and wrong beacon details in AP mode
- Observed MQTT Rx is not able to resume after rejoin in wifi\_station\_ble\_provisioning\_aws demo
- In Power save, user needs to increase application stack size for the Wi-Fi - AWS Device Shadow demo
- Observed DUT unable to wake up from Wi-Fi powersave deep sleep in case of without RAM retention over NCP UART
- Observed issue with the firmware update over the TCP in dense environment
- Observed socket close is not working as expected for TLS socket when socket connect, send data and socket close are performing in a continuous loop
- firmware\_flashing\_from\_host\_uart\_xmodem example fails to communicate over UART
- Observed UDP sendto API failure in wifi\_concurrent\_http\_server\_provisioning\_ncp application
- Observed connection issue with 3rd party AP In concurrent mode using webpage
- Documentation not added for customized socket options
- Observed data not received simultaneously when two sockets call recv() from two different RTOS tasks.
- WMM-PS/UAPSD is not supported

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA connection with the WPA3 using the Hunting and Pecking algorithm takes approximately 3-4 seconds.
- Connection failures have been observed with certain APs in environments with high channel congestion (~50-60% occupancy in an open lab).
- Region selection based on country IE in the beacon is not supported for ICs.
- Intermittent beacon reception from Access Point (beacon misses) occurs when channel congestion exceeds 85%.
- Uplink MU-MIMO is not supported
- When scanning with low power mode enabled, a sensitivity degradation of 3-6dB is observed, which may prevent APs at longer ranges from appearing in the scan results.
- For ICs, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported
- For modules, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported in PER mode.
- Observed ~2% increase in listen current and ~1% increase in standby associated current.
- Tx max powers for EVM limited data rates (like MCS7, MCS6, 54M, etc) will be reduced by 0.5dB.

**Access Point (AP) Mode**

- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected

**WPA2 Enterprise security (STA)**

- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789"

**Wi-Fi Concurrency ( AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios
- In concurrent mode, if IP is configured for AP mode ahead of STA mode then IPv6 configuration may fail for STA mode
- In concurrent mode, data transfer using the Link-local address will always use the first IP interface created by the application.
- In concurrent mode, 917 AP cannot process de-authentication frames sent by third-party STA if 917 STA is connected to WPA2+WPA3 enabled AP.

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled Low density parity check coding

**MU-MIMO (DL)**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test
- Observed Performance, Interop issues with MU MIMO with certain APs
- Less throughput was observed in MU-MIMO with some APs that enabled Low density parity check coding

**MU-MIMO (UL)**

- UL MU-MIMO is not supported

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process

**IPv4/IPv6**

- IP change notification is not indicated to the application
- In concurrent mode with dual IP, if the STA starts after AP is up, the STA IP configuration may fail for DHCP stateless mode
- concurrent\_firmware\_update\_from\_host\_uart example have stability issues during rejoin or disconnection process
- In concurrent mode, data transfer using the Link-local address will always use the first IP interface created by the application

**BSD Socket API**

- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported
- Observing issues with TCP retries when power save mode is enabled, especially when the module is in idle state
- TCP maximum retry value is valid upto 31

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt
- Secure SSL renegotiation not supported in Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads

**SNTP**

- Unable to get SNTP async events when CoEx mode and power save are enabled 

**Throughputs & Performance**

- Observed 20% less Wi-Fi throughput with SDK 3.x compare to target throughput (depends on host and host interface), SDK refinements are in progress

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations
- Observed OTA failures with power save enabled, So recommended to disable power save during OTA

**Wi-Fi IOT Cloud integration**

- **AWS IOT Core**
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
  - Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length
- **AZURE IOT Core**
  - Observed DUT after sending the data, its not sending the MQTT keep alive packet due to this Azure HUB closing the connection when power save is enabled

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with Powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency  is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 security

### **BLE**

**GAP**

- SPI interrupt miss issue was observed that prevents the host from receiving packets delivered by the TA, when there is continuous BLE TX/RX data transfer using the ble\_multiconnection\_gatt\_test application
- The DUT is unable to move to active state after BLE link loss using SiWN917Y100LGNB4 OPN

**AE** 

- Observed DUT hang issue while running TX notifications in peripheral role.

**Performance**

- The DUT hangs when the SRRC region is set in the ICs. However, this issue does not occur with the SiWN917Y module.

### **Multi-protocol**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled
- While executing Wi-Fi Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application
- Observed BLE bonding failure during continuous HTTPS download
- Observed BLE and WLAN connection failure with SMP, when WLAN connect and HTTPS GET called in a loop
- Observed BLE disconnection during wifi commissioning with few android mobiles
- Observed data stall on the remote machine during TCP/UDP transmission with power save enabled in  wifi\_throughput\_ble\_dual\_role\_ncp example
- Observed data stalls on remote server during Continuous TCP TX in wifi\_throughput\_ble\_dual\_role\_ncp example.
- For wifi\_ble\_powersave\_coex application, with 352K memory, observed 0xff2c - Memory limit exceeded in the given operating mode error.
- Observed throughput is not displaying for every interval of 'TEST\_TIMEOUT' when CONTINUOUS\_THROUGHPUT enabled for wifi\_station\_ble\_throughput\_app example.

### **Simplicity Studio and Commander (For EFR Host)**

- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain
- Universal Configurator (UC) for EFR32xG Products in NCP Mode is not supported

## **Limitations and Unsupported Features**

### **System**

- None

### **SDK**

- Baremetal mode is not supported
- WiSeConnect3\_SDK\_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade
- Lite Wireless firmware image is not supported for NCP mode.
- Zephyr is not supported

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- A maximum of 3 SSL connections are supported in Wi-Fi alone and CoEx modes. No.of  SSL Sockets in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
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
- In Wi-Fi Transceiver mode, MAC level encryption/decryption is not supported. 

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices
- BLE Slave latency value is valid up to 32 only
- BLE TX/RX throughput is less when tested with EFM as compared to EFR
- Maximum supported AE data length is 200 bytes
- Supports only two ADV\_EXT sets
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE
- Advertising Extension feature is not supported in Coexistence
- Isochronous channels feature is not supported
- Connection subrating feature is not supported
- LE power controller feature is not supported
- EATT feature is not supported
- Periodic Advertising with response(PAwR) feature is not supported
- BLE Audio is not supported
- The feature of dynamically changing the TX power when extended advertising is active is not supported
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address
- The maximum BLE power has been reduced by 2dB compared to the Datasheet Number, which will be addressed in the upcoming 3.3.1 patch release
- ICs do not support DEFAULT\_REGION and IGNORE\_REGION region codes.
- Modules do not support DEFAULT\_REGION and IGNORE\_REGION region codes in PER mode.

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported.

> **Note:** 
> 
> The following BLE Synchronous API's will be deprecated soon and the equivalent Asynchronous API's will be used instead in all BLE applications :
> 
> |**S.No**|**BLE Synchronous API's** |**BLE Asynchronous API's** |
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

# **WiSeConnect3\_SDK\_3.3.3 NCP Release Notes**   

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|10th October 2024|
|SDK Version|3\.3.3|
|Firmware Version|Standard: 1711.2.12.3.3.0.3|
|GSDK/SiSDK Version|SiSDK 2024.6.2 |
|Studio Version|5\.9.3.0|
|Release Package Name|WiSeConnect3\_SDK\_3.3.3|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components
  - Standard Wireless Firmware -  SiWx917 Firmware Binary available as SiWG917-B.2.12.3.3.0.3.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode.

Note:

- The release packages will have bug-fixes, enhancements, and new features in both 'SDK' and 'Firmware'. Customer shall update and use 'SDK' and 'Firmware' of same release package. SDK and FW combinations that are not released together are not supported.

## **Supported Hardware OPNs**

|**Hardware**|**OPN (Ordering Part Number)**|
| :- | :- |
|IC OPN|<p>QFN OPN: SiWN917M100LGTBA (Wi-Fi 6 NCP IC, QFN 7x7, 2.4 GHz, 4MB stacked flash, -40 to +85C​) </p><p>Module OPN: SiWN917Y100LGNBx</p>|
|Expansion kits:|<p>SiWx917-EB4346A (based on Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)</p><p>Module Board: SiW917Y-RB4357A (SiWN917Y Module Wi-Fi 6 and Bluetooth LE 4MB Flash RF-Pin Co-Processor Radio Board)</p>|

## **Supported Features** 

<table><tr><th><b>Section</b></th><th><b>Sub-Section</b></th><th><b>Feature</b></th></tr>
<tr><td rowspan="9">System</td><td rowspan="5">Operating modes</td><td>Wi-Fi STA (802.11ax, 802.11n)</td></tr>
<tr><td>Wi-Fi 802.11n AP</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + BLE</td></tr>
<tr><td>Wi-Fi Transceiver (802.11 b/g)</td></tr>
<tr><td>Security </td><td>Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Debug Lock, Flash Protection</td></tr>
<tr><td>Secure firmware upgrade options</td><td><p>- Firmware loading through UART, SPI Interface</p><p>- Secure Over the Air (OTA) Upgrade</p><p>- Firmware update via Bootloader</p></td></tr>
<tr><td>Crypto Support</td><td><p>- Crypto API's for Hardware Accelerators: Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)</p><p>- Software Accelerators: RSA, ECC</p><p>- Wrapping Secret keys (Symmetric crypto). </p><p>- Added ECDSA Sign and Verify APIs</p></td></tr>
<tr><td>System Power Save</td><td><p>- Deep Sleep with RAM retention and without RAM retention. </p><p>- Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. </p></td></tr>
<tr><td rowspan="13">Wi-Fi</td><td>Wi-Fi Protocols</td><td>IEEE 802.11 b/g/n/ax (2.4GHz)</td></tr>
<tr><td>Access Point (AP) Mode</td><td><p>- 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)</p><p>- Wi-Fi Security </p><p>- WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2) </p></td></tr>
<tr><td>Wi-Fi Scan</td><td>Selective Scan, Active/Passive Scan</td></tr>
<tr><td>Wi-Fi STA (Security Modes)</td><td>Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)</td></tr>
<tr><td>WPA2 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td></tr>
<tr><td>Wi-Fi STA Rejoin</td><td></td></tr>
<tr><td>Wi-Fi STA Roaming </td><td>BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication</td></tr>
<tr><td>Wi-Fi Protocol Power Save </td><td>Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP, TWT</td></tr>
<tr><td>QoS</td><td>WMM-QoS</td></tr>
<tr><td>Wi-Fi 6 Feature</td><td>MU-MIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID</td></tr>
<tr><td>Wi-Fi Concurrency </td><td>AP+STA (Same channel)</td></tr>
<tr><td>Wi-Fi Band/Channels</td><td>2.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14</td></tr>
<tr><td>Known Security Vulnerabilities Handled</td><td>WPA2 KRACK Attacks, Fragment and Forge Vulnerability</td></tr>
<tr><td rowspan="4">Network stack</td><td>Core Networking Features</td><td><p>- IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6</p><p>- SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 </p><p>- SSL server versions TLSV1.0 and TLSV1.2</p><p>- DHCPv4 Client,DHCPv6 Client</p><p>- DHCPv4 Server,DHCPv6 Server</p><p>- TCP/IP Bypass (LWIP as Hosted stack for reference)</p></td></tr>
<tr><td>Advanced Network Features</td><td>- HTTP Client/HTTPS Client/DNS Client/SNTP Client, Embedded MQTT, MQTT on host, IGMP</td></tr>
<tr><td>Wi-Fi IoT Cloud Integration</td><td><p>- AWS IoT Core</p><p>- Azure IoT Core</p></td></tr>
<tr><td>BSD and IoT sockets application programming interface(API)</td><td></td></tr>
<tr><td rowspan="2">BLE </td><td>Legacy features</td><td><p>- GAP(Advertising, Scanning, initiation, Connection and Bonding)</p><p>- Generic Attribute Protocol(GATT)</p><p>- Attribute protocol(ATT)</p><p>- Security</p><p>- LL Privacy 1.2</p><p>- Accept list</p><p>- Directed Advertising</p><p>- LE PHY(1Mbps, 2Mbps) & Coded PHY(125kbps, 500kbps)</p><p>- Simultaneous scanning on 1Mbps and Coded PHY</p><p>- LE dual role topology</p><p>- LE data packet length extensions(DLE)</p><p>- Asymmetric PHYs</p><p>- LE channel selection algorithm 2 (CSA#2)</p><p>- LE Secure connections</p><p>- Bluetooth 5.4 Qualified</p></td></tr>
<tr><td>Advertising Extensions </td><td><p>- Extended Advertising</p><p>- Periodic Advertising</p><p>- Periodic Advertising scanning</p><p>- Extended Advertising scanning</p><p>- Periodic Advertising list</p><p>- LE periodic advertising synchronization</p></td></tr>
</table>

### **Development Environment**

- Simplicity Studio IDE (SV5.9.3.0 version) and Debugger Integration. Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details
- Recommended to install and use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.6.2
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-ncp-manufacturing-utility-user-guide" for more details
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)

### **SDK**

- Simplified and Unified DX for Wi-Fi API 
- Simplifies application development and presents clean and standardized APIs
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA CoExistence**

- 3 wire CoEx acting as Wi-Fi with external Bluetooth 
- 3 wire CoEx acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.3.2 Release**

### **System**

- **Enhancements / New features**
  - Added limited anti-rollback feature for firmware update
- **Fixed Issues**
  - None
- **Documentation**
  - **None**

### **SDK**

- **Enhancements / New features**
  - Moved powersave\_standby\_associated example from featured  to featured/low\_power  section.
  - Updated aws\_starfield\_ca.pem.h certificate.(includes Starfield G2 Root CA).
  - Support for SL\_WIFI\_SCAN\_TYPE\_EXTENDED has been added to the sl\_wifi\_scan\_type\_t and sl\_wifi\_get\_stored\_scan\_results() API to enable retrieval of extended scan results.
- **Fixed Issues**
  - Fixed sl\_mqtt\_client\_connect() API to handle WLAN disconnection during MQTT connection.
  - Fixed SOFT\_AP\_PSK and SOFT\_AP\_SSID defines to have generic default PSK and SSID for AP in cli\_demo example.
  - Fixed certificate loading errors when No Optimization  compiler option is enabled in example applications.
  - Fixed handling of sl\_calib\_write command in wifi\_calibration example application.
  - Fixed handling of Wi-Fi client disconnections in concurrent mode for socket connections.
  - Fixed the issue where the DUT takes 2-4 minutes to reconnect with the AP and the Si-connect app shuts down in mid-iterations.
  - Fixed power\_save\_deep\_sleep example failures when STANDBY\_POWER\_SAVE\_WITH\_RAM\_RETENTION is enabled
- **Documentation**
  - Added usage note for SL\_WIFI\_MAX\_SCANNED\_AP.
  - Updated the documentation for sl\_wifi\_start\_scan API to add more information about SL\_WIFI\_SCAN\_TYPE\_EXTENDED.
  - Updated cli\_demo example readme with SOFT\_AP\_PSK and SOFT\_AP\_SSID defines.

### **Wi-Fi/Network Stack**

- **Enhancements / New features**
  - Added support for ALPN extension for TLS protocol.
  - Support added for SRRC region configuration.
  - CRYPTO\_HARDWARE is now supported during rejoin process.
  - Implemented region configuration restrictions for SiWN917Y modules.
  - 11ax receive performance improvement with transmit ppm.
  - Improved receive sensitivity performance in the presence of transmitter carrier frequency offset (CFO).
- **Fixed Issues**
  - Fixed vulnerabilities in NetX (CVE-2023-48315 ,CVE-2023-48316 ,CVE-2023-48691 ,CVE-2023-48692 )
- **Documentation**
  - Documentation updated for SRRC and Region configurations control.
  - Documentation updated for ALPN extension for the TLS protocol.

### **BLE**

- **Enhancements / New features**
  - Limited the Transmit power to 4dBm for WORLDWIDE and SRRC regions, and removed the option for users to modify BLE Gain tables for the SiWN917Y modules
- **Fixed Issues**
  - Removed the BLE packet transmission on channel-39 (2480MHz) at a 2Mbps data rate. 
  - Fixed BLE HP chain RX issues across temperature on few parts through improvements in bootup calibration.
  - Improved BLE transmit modulation parameters and harmonic emissions on the 8dBm transmission path. As a result, the active current of BLE transmit (8dBm path) can increase by around 1mA.
  - Enhanced the packet reception performance in the BLE LP chain.
  - Enhanced the BLE TX power in HP chain to align with the datasheet numbers.
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

- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP
- Memory configuration for NCP mode is 672K\_M4SS\_0K
- Set the following recommended FreeRTOS configuration in FreeRTOSConfig.h
  - configTIMER\_TASK\_PRIORITY to 55 
  - configTOTAL\_HEAP\_SIZE to 51200
  - configUSE\_POSIX\_ERRNO to 1

### **Wi-Fi/Network Stack**

- It is recommended to enable SL\_SI91X\_EXT\_TCP\_IP\_WAIT\_FOR\_SOCKET\_CLOSE BIT(16) of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer
- For high throughputs,  it is recommended to enable BIT(2) - SL\_SI91X\_FEAT\_AGGREGATION  of feature\_bit\_map in opermode. 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative
  - In case of TWT in CoEx mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
  - DUT keepalive should be configured aligned with AP keepalive in TWT modes.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly
- The minimum keep alive interval  value recommended for embedded MQTT is 10 Seconds
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published while using Embedded MQTT, else it leads to return wrong error code(0x21).
- In concurrent mode with dual IP, it is advised to bring up STA first (IP configuration) and AP later
- It is recommended to configure Tx, Rx, Global buffer pool ratio in the buffer config command for all Wi-Fi Socket operations from the host
- It is recommended to use "TCP exponential backoff" configuration for congested channels
- It is recommended is to disable broadcast filter during TCP connection to avoid ARP resolution issues
- To avoid IOP issues, it is recommended to disable power save before Wi-Fi connection
- Enable BIT(10)  SL\_SI91X\_FEAT\_SSL\_HIGH\_STREAMING\_BIT in feature bitmap to increase TLS\_Rx throughputs.
- It is recommended to set region\_code as `IGNORE\_REGION` in boot configurations for ACx module boards except for PER mode.

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout
  - Ensure Wi-Fi activity consumes lower intervals

## **Known Issues of WiSeConnect3\_SDK\_3.3.3 Release**

### **System**

- None

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number
- Matter extension based applications are experiencing compatibility issues with WiseConnect SDK 3.2.0. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s)
- Asynchronous Azure MQTT is not supported, this will be addressed in up coming release(s)
- mDNS with IPV6 is not supported
- Power Save with TCP/IP is not supported for UART interface
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer
- Recommended to use valid length(<= 202 bytes) for topic to be published in the "Embedded MQTT" demo, else it leads to return wrong error code(0x21)
- Observed AWS\_DEVICE SHADOW LOGGING STATS is not working with Power Save enable
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow
- Low Power examples usage and documentation still under scope of improvement.
- In WPA3 transition security mode, observed sl\_wifi\_get\_wireless\_info() API is giving wrong security type and PSK in both client and AP mode
- Observed sl\_wifi\_get\_operational\_statistics() API is giving wrong opermode and wrong beacon details in AP mode
- Observed MQTT Rx is not able to resume after rejoin in wifi\_station\_ble\_provisioning\_aws demo
- In Power save, user needs to increase application stack size for the Wi-Fi - AWS Device Shadow demo
- Observed DUT unable to wake up from Wi-Fi powersave deep sleep in case of without RAM retention over NCP UART
- Observed issue with the firmware update over the TCP in dense environment
- Observed socket close is not working as expected for TLS socket when socket connect, send data and socket close are performing in a continuous loop
- firmware\_flashing\_from\_host\_uart\_xmodem example fails to communicate over UART
- Observed UDP sendto API failure in wifi\_concurrent\_http\_server\_provisioning\_ncp application
- Observed connection issue with 3rd party AP In concurrent mode using webpage
- Documentation not added for customized socket options
- Observed data not received simultaneously when two sockets call recv() from two different RTOS tasks.
- WMM-PS/UAPSD is not supported

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA connection with the WPA3 using the Hunting and Pecking algorithm takes approximately 3-4 seconds.
- Connection failures have been observed with certain APs in environments with high channel congestion (~50-60% occupancy in an open lab).
- Region selection based on country IE in the beacon is not supported for ICs.
- Intermittent beacon reception from Access Point (beacon misses) occurs when channel congestion exceeds 85%.
- Uplink MU-MIMO is not supported
- When scanning with low power mode enabled, a sensitivity degradation of 3-6dB is observed, which may prevent APs at longer ranges from appearing in the scan results.
- For ICs, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported
- For modules, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported in PER mode.
- Observed ~2% increase in listen current and ~1% increase in standby associated current.
- Tx max powers for EVM limited data rates (like MCS7, MCS6, 54M, etc) will be reduced by 0.5dB.

**Access Point (AP) Mode**

- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected

**WPA2 Enterprise security (STA)**

- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789"

**Wi-Fi Concurrency ( AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios
- In concurrent mode, if IP is configured for AP mode ahead of STA mode then IPv6 configuration may fail for STA mode
- In concurrent mode, data transfer using the Link-local address will always use the first IP interface created by the application.
- In concurrent mode, 917 AP cannot process de-authentication frames sent by third-party STA if 917 STA is connected to WPA2+WPA3 enabled AP.

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled Low density parity check coding

**MU-MIMO (DL)**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test
- Observed Performance, Interop issues with MU MIMO with certain APs
- Less throughput was observed in MU-MIMO with some APs that enabled Low density parity check coding

**MU-MIMO (UL)**

- UL MU-MIMO is not supported

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process

**IPv4/IPv6**

- IP change notification is not indicated to the application
- In concurrent mode with dual IP, if the STA starts after AP is up, the STA IP configuration may fail for DHCP stateless mode
- concurrent\_firmware\_update\_from\_host\_uart example have stability issues during rejoin or disconnection process
- In concurrent mode, data transfer using the Link-local address will always use the first IP interface created by the application

**BSD Socket API**

- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported
- Observing issues with TCP retries when power save mode is enabled, especially when the module is in idle state
- TCP maximum retry value is valid upto 31

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt
- Secure SSL renegotiation not supported in Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads

**SNTP**

- Unable to get SNTP async events when CoEx mode and power save are enabled 

**Throughputs & Performance**

- Observed 20% less Wi-Fi throughput with SDK 3.x compare to target throughput (depends on host and host interface), SDK refinements are in progress

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations
- Observed OTA failures with power save enabled, So recommended to disable power save during OTA

**Wi-Fi IOT Cloud integration**

- **AWS IOT Core**
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
  - Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length
- **AZURE IOT Core**
  - Observed DUT after sending the data, its not sending the MQTT keep alive packet due to this Azure HUB closing the connection when power save is enabled

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with Powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency  is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 security

### **BLE**

**GAP**

- SPI interrupt miss issue was observed that prevents the host from receiving packets delivered by the TA, when there is continuous BLE TX/RX data transfer using the ble\_multiconnection\_gatt\_test application
- The DUT is unable to move to active state after BLE link loss using SiWN917Y100LGNB4 OPN

**AE** 

- Observed DUT hang issue while running TX notifications in peripheral role.

**Performance**

- The DUT hangs when the SRRC region is set in the ICs. However, this issue does not occur with the SiWN917Y module.

### **Multi-protocol**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled
- While executing Wi-Fi Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application
- Observed BLE bonding failure during continuous HTTPS download
- Observed BLE and WLAN connection failure with SMP, when WLAN connect and HTTPS GET called in a loop
- Observed BLE disconnection during wifi commissioning with few android mobiles
- Observed data stall on the remote machine during TCP/UDP transmission with power save enabled in  wifi\_throughput\_ble\_dual\_role\_ncp example
- Observed data stalls on remote server during Continuous TCP TX in wifi\_throughput\_ble\_dual\_role\_ncp example.
- For wifi\_ble\_powersave\_coex application, with 352K memory, observed 0xff2c - Memory limit exceeded in the given operating mode error.
- Observed throughput is not displaying for every interval of 'TEST\_TIMEOUT' when CONTINUOUS\_THROUGHPUT enabled for wifi\_station\_ble\_throughput\_app example.

### **Simplicity Studio and Commander (For EFR Host)**

- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain
- Universal Configurator (UC) for EFR32xG Products in NCP Mode is not supported

## **Limitations and Unsupported Features**

### **System**

- None

### **SDK**

- Baremetal mode is not supported
- WiSeConnect3\_SDK\_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade
- Lite Wireless firmware image is not supported for NCP mode.
- Zephyr is not supported

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- A maximum of 3 SSL connections are supported in Wi-Fi alone and CoEx modes. No.of  SSL Sockets in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
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
- bTWT , Intra PPDU Power save, Spatial Re-Use, BSS coloring features not supported
- HTTPS server is not supported.
- In Wi-Fi Transceiver mode, MAC level encryption/decryption is not supported. 

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices
- BLE Slave latency value is valid up to 32 only
- BLE TX/RX throughput is less when tested with EFM as compared to EFR
- Maximum supported AE data length is 200 bytes
- Supports only two ADV\_EXT sets
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE
- Advertising Extension feature is not supported in Coexistence
- Isochronous channels feature is not supported
- Connection subrating feature is not supported
- LE power controller feature is not supported
- EATT feature is not supported
- Periodic Advertising with response(PAwR) feature is not supported
- BLE Audio is not supported
- The feature of dynamically changing the TX power when extended advertising is active is not supported
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address
- The maximum BLE power has been reduced by 2dB compared to the Datasheet Number, which will be addressed in the upcoming 3.3.1 patch release
- ICs do not support DEFAULT\_REGION and IGNORE\_REGION region codes.
- Modules do not support DEFAULT\_REGION and IGNORE\_REGION region codes in PER mode.

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported.

> **Note:** 
> 
> The following BLE Synchronous API's will be deprecated soon and the equivalent Asynchronous API's will be used instead in all BLE applications :
> 
> |**S.No**|**BLE Synchronous API's** |**BLE Asynchronous API's** |
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

# **WiSeConnect3\_SDK\_3.3.2 NCP Release Notes**   

## **Release Details**


|**Item**|**Details**|
| :- | :- |
|Release date|5th September, 2024|
|SDK Version|3\.3.2|
|Firmware Version|Standard: 1711.2.12.2.1.0.9|
|GSDK/SiSDK Version|SiSDK 2024.6.1|
|Studio Version|5\.9.2.0|
|Release Package Name|WiSeConnect3\_SDK\_3.3.2|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components
  - Standard Wireless Firmware -  SiWx917 Firmware Binary available as SiWG917-B.2.12.2.1.0.9.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode.

Note:

- The release packages will have bug-fixes, enhancements, and new features in both 'SDK' and 'Firmware'. Customer shall update and use 'SDK' and 'Firmware' of same release package. SDK and FW combinations that are not released together are not supported.

## **Supported Hardware OPNs**

|**Hardware**|**OPN**|
| :- | :- |
|Silicon (IC OPN)|<p>QFN OPN: SiWN917M100LGTBA (Wi-Fi 6 NCP IC, QFN 7x7, 2.4 GHz, 4MB stacked flash, -40 to +85C​) </p><p>Module OPN: SiWN917Y100LGNBx</p>|
|Expansion kits:|<p>SiWx917-EB4346A (based on Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)</p><p>Module Board: SiW917Y-RB4357A (SiWN917Y Module Wi-Fi 6 and Bluetooth LE 4MB Flash RF-Pin Co-Processor Radio Board)</p>|

## **Supported Features** 

<table><tr><th><b>Section</b></th><th><b>Sub-Section</b></th><th><b>Feature</b></th></tr>
<tr><td rowspan="9">System</td><td rowspan="5">Operating modes</td><td>Wi-Fi STA (802.11ax, 802.11n)</td></tr>
<tr><td>Wi-Fi 802.11n AP</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + BLE</td></tr>
<tr><td>Wi-Fi Transceiver (802.11 b/g)</td></tr>
<tr><td>Security </td><td>Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Debug Lock, Flash Protection</td></tr>
<tr><td>Secure firmware upgrade options</td><td><p>- Firmware loading through UART, SPI Interface</p><p>- Secure Over the Air (OTA) Upgrade</p><p>- Firmware update via Bootloader</p></td></tr>
<tr><td>Crypto Support</td><td><p>- Crypto API's for Hardware Accelerators: Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)</p><p>- Software Accelerators: RSA, ECC</p><p>- Wrapping Secret keys (Symmetric crypto). </p><p>- Added ECDSA Sign and Verify APIs</p></td></tr>
<tr><td>System Power Save</td><td><p>- Deep Sleep with RAM retention and without RAM retention. </p><p>- Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. </p></td></tr>
<tr><td rowspan="13">Wi-Fi</td><td>Wi-Fi Protocols</td><td>IEEE 802.11 b/g/n/ax (2.4GHz)</td></tr>
<tr><td>Access Point (AP) Mode</td><td><p>- 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)</p><p>- Wi-Fi Security </p><p>` `- WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2) </p></td></tr>
<tr><td>Wi-Fi Scan</td><td>Selective Scan, Active/Passive Scan</td></tr>
<tr><td>Wi-Fi STA (Security Modes)</td><td>Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)</td></tr>
<tr><td>WPA2 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td></tr>
<tr><td>Wi-Fi STA Rejoin</td><td></td></tr>
<tr><td>Wi-Fi STA Roaming </td><td>BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication</td></tr>
<tr><td>Wi-Fi Protocol Power Save </td><td>Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP, TWT</td></tr>
<tr><td>QoS</td><td>WMM-QoS</td></tr>
<tr><td>Wi-Fi 6 Feature</td><td>MUMIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID</td></tr>
<tr><td>Wi-Fi Concurrency </td><td>AP+STA (Same channel)</td></tr>
<tr><td>Wi-Fi Band/Channels</td><td>2\.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14</td></tr>
<tr><td>Known Security Vulnerabilities Handled</td><td>WPA2 KRACK Attacks, Fragment and Forge Vulnerability</td></tr>
<tr><td rowspan="4">Network stack</td><td>Core Networking Features</td><td><p>- IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6</p><p>- SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 </p><p>- SSL server versions TLSV1.0 and TLSV1.2</p><p>- DHCPv4 Client,DHCPv6 Client</p><p>- DHCPv4 Server,DHCPv6 Server</p><p>- TCP/IP Bypass (LWIP as Hosted stack for reference)</p></td></tr>
<tr><td>Advanced Network Features</td><td>- HTTP Client/HTTPS Client/DNS Client/SNTP Client, Embedded MQTT/MQTT on host, IGMP</td></tr>
<tr><td>Wi-Fi IoT Cloud Integration</td><td><p>- AWS IoT Core</p><p>- Azure IoT Core</p></td></tr>
<tr><td>BSD and IoT sockets application programming interface(API)</td><td></td></tr>
<tr><td rowspan="2">BLE </td><td>Legacy features</td><td><p>- GAP(Advertising, Scanning, initiation, Connection and Bonding)</p><p>- Generic Attribute Protocol(GATT)</p><p>- Attribute protocol(ATT)</p><p>- Security</p><p>- LL Privacy 1.2</p><p>- Accept list</p><p>- Directed Advertising</p><p>- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)</p><p>- Simultaneous scanning on 1Mbps and Coded PHY</p><p>- LE dual role topology</p><p>- LE data packet length extensions(DLE)</p><p>- Asymmetric PHYs</p><p>- LE channel selection algorithm 2 (CSA#2)</p><p>- LE Secure connections</p><p>- Bluetooth 5.4 Qualified</p></td></tr>
<tr><td>Advertising Extensions </td><td><p>- Extended Advertising</p><p>- Periodic Advertising</p><p>- Periodic Advertising scanning</p><p>- Extended Advertising scanning</p><p>- Periodic Advertising list</p><p>- LE periodic advertising synchronization</p></td></tr>
</table>

### **Development Environment**

- Simplicity Studio IDE (SV5.9.2.0 version) and Debugger Integration. Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details
- Recommended to install and use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.6.1
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-ncp-manufacturing-utility-user-guide" for more details
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)

### **SDK**

- Simplified and Unified DX for Wi-Fi API 
- Simplifies application development and presents clean and standardized APIs
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA CoExistence**

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

### **SDK**

- **Enhancements / New features**
  - Added Ozone configuration files.
- **Fixed Issues**
  - None
- **Documentation**
  - Enhanced description, limitations, exceptions of APIs, types, and constants across all modules of the API Reference Guide.
  - Replaced references of “TA” with “NWP” and “ThreadArch” with “Network Wireless Processor”
  - Added steps to use Ozone debugger.

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

- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP
- Memory configuration for NCP mode is 672K\_M4SS\_0K
- Set the following recommended FreeRTOS configuration in FreeRTOSConfig.h
  - configTIMER\_TASK\_PRIORITY to 55 
  - configTOTAL\_HEAP\_SIZE to 51200
  - configUSE\_POSIX\_ERRNO to 1

### **IDE for EFR host**

- Simplicity Studio. This release is tested with SV5.9.1.0 version
- Recommended to use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.6.0 for Silicon labs modules
- Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details

### **Wi-Fi/Network Stack**

- It is recommended to enable SL\_SI91X\_EXT\_TCP\_IP\_WAIT\_FOR\_SOCKET\_CLOSE BIT(16) of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative
  - In case of TWT in coex mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
  - DUT keepalive should be configured aligned with AP keepalive in TWT modes.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly
- The minimum keep alive interval  value recommended for embedded MQTT is 10 Seconds
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save
- In concurrent mode with dual IP, it is advised to bring up STA first (IP configuration) and AP later
- It is recommended to configure Tx ,Rx , Global buffer pool ratio in the buffer config command for all Wi-Fi Socket operations from the host
- It is recommended to use "TCP exponential backoff" configuration for congested channels
- It is recommended is to disable broadcast filter during TCP connection to avoid ARP resolution issues
- To avoid IOP issues, it is recommended to disable power save before Wi-Fi connection
- Need to enable below in feature bit map for TLS\_RX to increase throughputs SL\_SI91X\_FEAT\_SSL\_HIGH\_STREAMING\_BIT - BIT(10)

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout
  - Ensure Wi-Fi activity consumes lower intervals

## **Known Issues of WiSeConnect3\_SDK\_3.3.2 Release**

### **System**

- None

### **SDK**

- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number
- Matter extension based applications are experiencing compatibility issues with WiseConnect SDK 3.2.0. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s)
- Asynchronous Azure MQTT is not supported, this will be addressed in up coming release(s)
- mDNS with IPV6 is not supported
- Power Save with TCP/IP is not supported for UART interface
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer
- Recommended to use valid length(<= 202 bytes) for topic to be published in the "Embedded MQTT" demo, else it leads to return wrong error code(0x21)
- Observed AWS\_DEVICE SHADOW LOGGING STATS is not working with Power Save enable
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow
- In WPA3 transition security mode, observed sl\_wifi\_get\_wireless\_info() API is giving wrong security type and PSK in both client and AP mode
- Observed sl\_wifi\_get\_operational\_statistics() API is giving wrong opermode and wrong beacon details in AP mode
- Observed MQTT Rx is not able to resume after rejoin in wifi\_station\_ble\_provisioning\_aws demo
- In Power save, user needs to increase application stack size for Wi-Fi - AWS Device Shadow demo
- Observed issue with Wi-Fi - Powersave Deep Sleep in case of without RAM retention
- Observed issue with the firmware update over the TCP in dense environment
- Observed socket close is not working as expected for TLS socket when socket connect, send data and socket close are performing in a continuous loop
- Below are the list of applications which are not working as expected:
  - http\_otaf\_twt 
  - firmware\_flashing\_from\_host\_uart\_xmodem
- Observed UDP sendto API failure in wifi\_concurrent\_http\_server\_provisioning\_ncp application
- Observed connection issue with 3rd party AP In concurrent mode using webpage
- Documentation not added for customized socket options
- Observed data not received from last socket while sending from multiple TCP client sockets
- WMM-PS/UAPSD is not supported

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds
- Observed connection failures with some APs in more channel congestion ~50-60% occupancy open lab environment
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) when channel congestion > 85%
- UL MU-MIMO is not supported
- While scanning with low power mode enable, sensitivity degradation of 3-6dB is observed. Due to this AP's that are at longer range might not be shown in scan results
- Passive scan is failing when DUT is configured in world domain.

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected

**WPA2 Enterprise security (STA)**

- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789"

**Wi-Fi Concurrency ( AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios
- In concurrent mode, if IP is configured for AP mode ahead of STA mode then IPv6 configuration may fail for STA mode
- In concurrent mode, data transfer using the Link-local address will always use the first IP interface created by the application

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled Low density parity check coding

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test
- Observed Performance, Interop issues with MU MIMO with certain APs
- Less throughput was observed in MU-MIMO with some APs that enabled Low density parity check coding

**MU-MIMO (UL)**

- UL MU-MIMO is not supported

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process

**IPv4/IPv6**

- IP change notification is not indicated to the application
- In concurrent mode with dual IP, if the STA starts after AP is up, the STA IP configuration may fail for DHCP stateless mode
- The example application concurrent\_firmware\_update\_from\_host\_uart has stability issues during rejoin or disconnection process
- In concurrent mode, data transfer using the Link-local address will always use the first IP interface created by the application

**BSD Socket API**

- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported
- Observing issues with TCP retries when power save mode is enabled, especially when the module is in idle state
- TCP maximum retry value is valid upto 31

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt
- Secure SSL renegotiation not supported in Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads

**SNTP**

- Unable to get SNTP async events when coex mode and power save are enabled 

**Throughputs & Performance**

- Observed 20% less Wi-Fi throughput with SDK 3.x compare to target throughput (depends on host and host interface), SDK refinements are in progress

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations
- Observed OTA failures with power save enabled, So recommended to disable power save during OTA

**Wi-Fi IOT Cloud integration**

- **AWS IOT Core**
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
  - Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length
- **AZURE IOT Core**
  - Observed DUT after sending the data, its not sending the MQTT keep alive packet due to this Azure HUB closing the connection when power save is enabled

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with Powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency  is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 security

### **BLE**

**GAP**

- SPI interrupt miss issue was observed that prevents the host from receiving packets delivered by the TA, when there is continuous BLE TX/RX data transfer using the ble\_multiconnection\_gatt\_test application

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm and not to use power\_index 127 for BLE HP chain

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled
- While executing Wi-Fi Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application
- Observed BLE bonding failure during continuous HTTPS download
- Observed BLE and WLAN connection failure with SMP, when WLAN connect and HTTPS GET called in a loop
- Observed BLE disconnection during wifi commissioning with few android mobiles
- Observed data stall on the remote machine during TCP/UDP transmission with power save enabled in  wifi\_throughput\_ble\_dual\_role\_ncp example
- Observed issue in displaying throughput for interval of 'TEST\_TIMEOUT' when CONTINUOUS\_THROUGHPUT enabled in wifi\_station\_ble\_throughput\_app demo

### **Simplicity Studio and Commander (For EFR Host)**

- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain
- Universal Configurator (UC) for EFR32xG Products in NCP Mode is not supported

## **Features in RoadMap**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities
- SDIO interface support in NCP mode

### **System**

- Secure communication over host interfaces (UART, SPI)
- UART 2 Debug prints are supported only on ULP\_GPIO\_9.

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh(4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**

### **System**

- None

### **SDK**

- Baremetal mode is not supported
- WiSeConnect3\_SDK\_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade
- Lite firmware image is not supported for NCP mode.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- A maximum of 3 SSL connections are supported in Wi-Fi alone and CoEx modes.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- The maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.
- AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
- In concurrent AP mode, aggregation (Tx/Rx) is not supported.
- Embedded HTTP Server is not supported.
- mDNS with IPV6 is not supported.
- In Wi-Fi Transceiver mode, MAC level encryption/decryption is not supported. 
- Low power scan supports 1 mbps packets reception only.
- Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices
- BLE Slave latency value is valid up to 32 only
- BLE TX/RX throughput is less when tested with EFM as compared to EFR
- Maximum supported AE data length is 200 bytes
- Supports only two ADV\_EXT sets
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE
- Advertising Extension feature is not supported in Coexistence
- Isochronous channels feature is not supported
- Connection subrating feature is not supported
- LE power controller feature is not supported
- EATT feature is not supported
- Periodic Advertising with response(PAwR) feature is not supported
- BLE Audio is not supported
- The feature of dynamically changing the TX power when extended advertising is active is not supported
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address
- The maximum BLE power has been reduced by 2dB compared to the Datasheet Number, which will be addressed in the upcoming 3.3.1 patch release

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported

> **Note:** 
> 
> The following BLE Synchronous API's will be deprecated soon and the equivalent Asynchronous API's will be used instead in all BLE applications :
> 
> |**S.No**|**BLE Synchronous API's** |**BLE Asynchronous API's** |
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

# **WiSeConnect3\_SDK\_3.3.1 NCP Release Notes**   

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|14th August, 2024|
|SDK Version|3\.3.1|
|Firmware Version|Standard: 1711.2.12.2.1.0.9|
|GSDK/SiSDK Version|SiSDK 2024.6.1|
|Studio Version|5.9.2.0|
|Release Package Name|WiSeConnect3\_SDK\_3.3.1|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components
  - Standard Wireless Firmware -  SiWx917 Firmware Binary available as SiWG917-B.2.12.2.1.0.9.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode.
> Note:
>   - The latest releases will have bug-fixes, enhancements, and new features in both 'SDK' and 'Firmware'. Hence, it is recommended to update and use 'SDK' and 'Firmware' of same release package.

## **Supported Hardware OPNs**

|**Hardware**|**OPN**|
| :- | :- |
|Silicon (IC OPN)|<p>QFN OPN: SiWN917M100LGTBA (Wi-Fi 6 NCP IC, QFN 7x7, 2.4 GHz, 4MB stacked flash, -40 to +85C​) </p><p>Module OPN: SiWN917Y100LGNBx</p>|
|Expansion kits: |<p>SiWx917-EB4346A (based on Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)</p><p>Module Board: SiW917Y-RB4357A (SiWN917Y Module Wi-Fi 6 and Bluetooth LE 4MB Flash RF-Pin Co-Processor Radio Board)</p>|

## **Supported Features** 

<table><tr><th><b>Section</b></th><th><b>Sub-Section</b></th><th><b>Feature</b></th></tr>
<tr><td rowspan="9">System</td><td rowspan="5">Operating modes</td><td>Wi-Fi STA (802.11ax, 802.11n)</td></tr>
<tr><td>Wi-Fi 802.11n AP</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + BLE</td></tr>
<tr><td>Wi-Fi Transceiver (802.11 b/g)</td></tr>
<tr><td>Security </td><td>Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Debug Lock, Flash Protection</td></tr>
<tr><td>Secure firmware upgrade options</td><td><p>- Firmware loading through UART, SPI Interface</p><p>- Secure Over the Air (OTA) Upgrade</p><p>- Firmware update via Bootloader</p></td></tr>
<tr><td>Crypto Support</td><td><p>- Crypto API's for Hardware Accelerators: Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)</p><p>- Software Accelerators: RSA, ECC</p><p>- Wrapping Secret keys (Symmetric crypto). </p><p>- Added ECDSA Sign and Verify APIs</p></td></tr>
<tr><td>System Power Save</td><td><p>- Deep Sleep with RAM retention and without RAM retention. </p><p>- Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. </p></td></tr>
<tr><td rowspan="13">Wi-Fi</td><td>Wi-Fi Protocols</td><td>IEEE 802.11 b/g/n/ax (2.4GHz)</td></tr>
<tr><td>Access Point (AP) Mode</td><td><p>- 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)</p><p>- Wi-Fi Security </p><p>&emsp;- WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2) </p></td></tr>
<tr><td>Wi-Fi Scan</td><td>Selective Scan, Active/Passive Scan</td></tr>
<tr><td>Wi-Fi STA (Security Modes)</td><td>Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)</td></tr>
<tr><td>WPA2 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td></tr>
<tr><td>Wi-Fi STA Rejoin</td><td></td></tr>
<tr><td>Wi-Fi STA Roaming </td><td>BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication</td></tr>
<tr><td>Wi-Fi Protocol Power Save </td><td>Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP, TWT</td></tr>
<tr><td>QoS</td><td>WMM-QoS</td></tr>
<tr><td>Wi-Fi 6 Feature</td><td>MUMIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID</td></tr>
<tr><td>Wi-Fi Concurrency </td><td>AP+STA (Same channel)</td></tr>
<tr><td>Wi-Fi Band/Channels</td><td>2\.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14</td></tr>
<tr><td>Known Security Vulnerabilities Handled</td><td>WPA2 KRACK Attacks, Fragment and Forge Vulnerability</td></tr>
<tr><td rowspan="4">Network stack</td><td>Core Networking Features</td><td><p>- IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6</p><p>- SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 </p><p>- SSL server versions TLSV1.0 and TLSV1.2</p><p>- DHCPv4 Client,DHCPv6 Client</p><p>- DHCPv4 Server,DHCPv6 Server</p><p>- TCP/IP Bypass (LWIP as Hosted stack for reference)</p></td></tr>
<tr><td>Advanced Network Features</td><td>- HTTP Client/HTTPS Client/DNS Client/SNTP Client, Embedded MQTT/MQTT on host, IGMP</td></tr>
<tr><td>Wi-Fi IoT Cloud Integration</td><td><p>- AWS IoT Core</p><p>- Azure IoT Core</p></td></tr>
<tr><td>BSD and IoT sockets application programming interface(API)</td><td></td></tr>
<tr><td rowspan="2">BLE </td><td>Legacy features</td><td><p>- GAP(Advertising, Scanning, initiation, Connection and Bonding)</p><p>- Generic Attribute Protocol(GATT)</p><p>- Attribute protocol(ATT)</p><p>- Security</p><p>- LL Privacy 1.2</p><p>- Accept list</p><p>- Directed Advertising</p><p>- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)</p><p>- Simultaneous scanning on 1Mbps and Coded PHY</p><p>- LE dual role topology</p><p>- LE data packet length extensions(DLE)</p><p>- Asymmetric PHYs</p><p>- LE channel selection algorithm 2 (CSA#2)</p><p>- LE Secure connections</p><p>- Bluetooth 5.4 Qualified</p></td></tr>
<tr><td>Advertising Extensions </td><td><p>- Extended Advertising</p><p>- Periodic Advertising</p><p>- Periodic Advertising scanning</p><p>- Extended Advertising scanning</p><p>- Periodic Advertising list</p><p>- LE periodic advertising synchronization</p></td></tr>
</table>

### **Development Environment**

- Simplicity Studio IDE (SV5.9.2.0 version) and Debugger Integration. Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details
- Recommended to install and use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.6.1
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-ncp-manufacturing-utility-user-guide" for more details
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)

### **SDK**

- Simplified and Unified DX for Wi-Fi API 
- Simplifies application development and presents clean and standardized APIs
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA CoExistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth 
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.3.0 Release**

### **System**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - Updated EFx32 RESET configurations by changing the reset pin GPIO setting to Open Drain

### **SDK**

- **Enhancements / New features**
  - The throughputs are improved by upgrading the STM32 Clock speed to 50MHz from the previous 42MHz
  - Added SiWx91x NCP connectivity FW Update Process using simplicity commander
  - Added support for both IPv4 and IPv6 config in sl\_net\_up()
  - Added TCP and UDP data transfer for WLAN concurrent mode example
  - Reintegrated I2C changes into the wifi\_station\_ble\_provisioning\_aws example
  - Refactored LWIP porting layer and examples
  - Renamed "Additional Status Codes" as "WiSeConnect Status Codes" in documentation
  - Added support in the Azure IoT example to publish messages continuously
  - Added support for static ip address in LWIP port
  - Added azureiotconfig KEEP\_ALIVE\_TIMEOUT\_SECONDS in preprocessor settings
  - Added documentation changes for sl\_net\_get\_credential API
  - Added new low power example powersave\_standby\_associated\_tcp\_client
  - Added the missing implementation for LOAD\_DEFAULT\_NWP\_FW\_ACTIVE\_LOW
  - Added Wi-Fi Coex - Wi-Fi Client HTTPS BLE Dual Role Readme file corrections
  - Added readme sections which are not clear
  - Added below new API's
    - sl\_si91x\_get\_firmware\_version,
    - sl\_si91x\_get\_firmware\_size, 
    - sl\_net\_set\_dns\_server, 
    - sl\_si91x\_is\_device\_initialized
  - Moved the applications to examples/featured/low\_power folder - power\_save\_deep\_sleep, twt\_tcp\_client
  - Updated readme files for power\_save\_deep\_sleep, twt\_tcp\_client, powersave\_standby\_associated applications
  - Updated the description of the output paramater "fw\_image\_size" in sl\_wifi\_get\_firmware\_size API
  - Updated image in readme changes in power\_save\_deep\_sleep application
  - Modified the brief of few APIs for better understanding
  - Updated TWT TCP client app and Readme documentation
  - Updated API documentation with missing error codes
  - Updated documentation for NCP SRAM Configurations and macros
  - Updated documentation changes for Host RTC timer APIs
  - Added steps to update firmware using Cygwin on windows PC
  - The structure variable no longer requires manual initialization before calling sl\_wifi\_get\_wireless\_info(). This is now handled automatically in the API.
- **Fixed Issues**
  - Removed the unused files from sl\_net/utilities folder
  - Fixed exception for HTTPS GET while running simple\_https\_server python script at server side
  - Fixed issue with simultaneous operation of accept and recv
  - Fixed stack overflow of ble\_central task by increasing stack size in wifi\_https\_ble\_dual\_role\_v6 Application
  - Fixed the "valid firmware not present" error in the SDK to recover the module from a stuck state
  - Fixed rejoin issue by adding channel\_bitmap in join request

### **Wi-Fi/Network Stack**

- **Enhancements / New features**
  - Reduced Tx power variation across the channels
  - Updated WLAN 11B temperature compensation Lookup table for modules
  - Added support for optional Customer Gain offset in Channel 14
  - Added the mempool buffer with quota in STM32 to increase Throughputs
  - Updated NCP mode interface details for readme files
  - Added support for Quick Scan Feature
  - Updated MQTT Documentation with topic length, password and username
  - Added support for IPV6 in MQTT
  - Updated data\_callback signature for async sockets
  - Added remote terminate Support to resolve the Socket reconnection delay issue in twt\_use\_case\_remote\_app demo
  - Added certificate index and TLS configuration support for MQTTS
  - Updated documentation for the sl\_wifi\_init and sl\_wifi\_deinit APIs
  - Updated mDNS API documentation
  - Updated documentation for SI91X\_TCP\_IP\_FEATURE\_BITMAP macro definitions
  - Added Aggregation and WPS Disable bits to improve AP mode throughput
  - Added front end switch bitmap (SL\_SI91X\_EXT\_FEAT\_FRONT\_END\_SWITCH\_PINS\_ULP\_GPIO\_4\_5\_0) in boot config for access point example
  - Updated readme of ap\_throughput example
  - Added support for TCP\_ACK\_INDICATION call back feature 
  - Added socket\_bitmap to update the TCP\_ACK\_INDICATION bitmap
  - Added missing certificate index handling in http\_otaf application
  - Added changes to control AP mode ACS survey according to the channel list specified for the user-provided region
  - Added configuration options for certificate index and TLS version in the MQTTs socket
  - Added support for channel 14 in PER mode.
  - Increased broadcast wait time from 14ms to 70ms and beacon wait time from 10ms to 25ms during powersave if broadcast filter is disabled
- **Fixed Issues**
  - Fixed the issue observed with scanning in AP mode
  - The minimum supported beacon missed count value has been updated to 40
  - Fixed DUT hang issue while running aws\_device\_shadow application with Power save enabled
  - Fixed issues related to NCP UART build configuration
  - Fixed compilation issues in the wifi\_firmware\_update\_uart\_ncp example after enabling SL\_SI91X\_UART\_HIGH\_SPEED\_ENABLE in preprocessor settings
  - Fixed wrong values for wlan\_state and channel\_number in sl\_wifi\_get\_wireless\_info structure
  - Fixed issue with multiple sockets handling accept calls.
  - Fixed DUT hang issue when WLAN disconnection happens during an HTTP POST operation
  - Fixed issue in sl\_si91x\_recv API by calling sl\_si91x\_recvfrom
  - Fixed documentation issues in sl\_si91x\_http\_otaf API
  - Fixed issue with sl\_si91x\_select API timeout value set to NULL
  - Fixed the http\_client application to return a failure response when an HTTP(s) GET request is made for a file not present on the server
  - Fixed MQTT connection failure with error code 0x10065 (SL\_STATUS\_SI91X\_ERROR\_WITH\_MQTT\_COMMAND) when http\_client functionality is added
  - Fixed issue with the MQTT publish API, which was not returning an error when the AP lacked internet connectivity
  - Fixed issues with MQTT state machine incase of Wi-Fi disconnection
  - Fixed the MQTT publish failure issue in AWS\_MQTT example
  - Fixed issue with sl\_si91x\_select API when configure socket\_select timeout set to '0'
  - Fixed issue with HTTPS download being terminated in the wifi\_https\_ble\_dual\_role\_v6 application
  - Fixed sl\_wifi\_init() failure with status 0x7
  - Fixed issue with MSS(Maximum Segment Size) error handling mechanism
  - Fixed issue with the client\_socket\_id when callback is not NULL
  - Implemented periodic TCP retries for HTTP sockets instead of exponential backoff
  - Fixed the SSL connection issue with the TLS13\_CHACHA20\_POLY1305\_SHA256 cipher
  - Fixed socket shutdown issues when running the socket create →  data transfer → shutdown sequence in a loop
  - Fixed instability issues with the DHCP server in IPv6
  - Fixed data transfer issues on the link-local address when a unicast global IPv6 address is not configured
  - Fixed instability issues with concurrent mode** in Dual IP
  - Fixed incorrect advertisement of WPA IE along with RSN IE
  - Fixed Wrap key functionality to provide accurate output for 192-bit keys in AES and GCM
  - Fixed LP chain DC calibration issue

### **BLE**

- **Enhancements / New features**
  - Added a few additional details to the readme of "ble\_ae\_peripheral", ble\_ae\_central, ble\_central, ble\_heart\_rate\_profile, ble\_longrange\_2mbps and ble\_power\_save demos
  - Improved margin on emissions in FCC region accounting part-to-part variation with BLE High performance chain. 
  - Improved transmit power vs power index linearity in BLE High Performance chain
- **Fixed Issues**
  - Resolved issue where the device stopped transmitting AE advertising packets after sometime , when configured 'Two ADV sets one handle with public address and another with Random address and Primary PHY =1M and Sec PHY =1M/2M"
  - Resolved issue where the device was not transmitting AUX\_SCAN\_REQ packets to a remote device configured in scannable extended advertising mode
  - Fixed the issue where device was unable to receive periodic advertising reports, when  "BLE\_AE\_PERODIC\_DUPLICATE\_FILTERING\_ENABLED" bit is enabled in AE option of the AE Periodic create sync API
  - Fixed the issue where device was unable to receive periodic advertising reports, when  "BLE\_AE\_PERIODIC\_LIST\_USED" bit is enabled in the AE option of the AE Periodic create sync API
  - Addressed the issue of high BLE power numbers. As a result, the power numbers now match the values specified in the datasheet
  - Fixed BLE HP transmit power to power index mapping 

### **Multi-protocol**

- **Enhancements / New features**
  - Implemented 5msec timeout for NULL DATA packet to reduce radio switch delay in dense environment
- **Fixed Issues**
  - Fixed a bug in DTIM beacon time computation during coexistence without power save**,** when releasing the radio without receiving the first beacon

## **Recommendations**

### **System**

- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP
- Memory configuration for NCP mode is 672K\_M4SS\_0K
- Set the following recommended FreeRTOS configuration in FreeRTOSConfig.h
  - configTIMER\_TASK\_PRIORITY to 55 
  - configTOTAL\_HEAP\_SIZE to 51200
  - configUSE\_POSIX\_ERRNO to 1

### **IDE for EFR host**

- Simplicity Studio. This release is tested with SV5.9.1.0 version
- Recommended to use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.6.0 for Silicon labs modules
- Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details

### **Wi-Fi/Network Stack**

- It is recommended to enable SL\_SI91X\_EXT\_TCP\_IP\_WAIT\_FOR\_SOCKET\_CLOSE BIT(16) of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative
  - In case of TWT in coex mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
  - DUT keepalive should be configured aligned with AP keepalive in TWT modes.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly
- The minimum keep alive interval  value recommended for embedded MQTT is 10 Seconds
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save
- In concurrent mode with dual IP, it is advised to bring up STA first (IP configuration) and AP later
- It is recommended to configure Tx ,Rx , Global buffer pool ratio in the buffer config command for all Wi-Fi Socket operations from the host
- It is recommended to use "TCP exponential backoff" configuration for congested channels
- It is recommended is to disable broadcast filter during TCP connection to avoid ARP resolution issues
- To avoid IOP issues, it is recommended to disable power save before Wi-Fi connection
- Need to enable below in feature bit map for TLS\_RX to increase throughputs
  SL\_SI91X\_FEAT\_SSL\_HIGH\_STREAMING\_BIT - BIT(10)

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout
  - Ensure Wi-Fi activity consumes lower intervals

## **Known Issues of WiSeConnect3\_SDK\_3.3.1 Release**

### **System**

- None

### **SDK**

- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number
- Matter extension based applications are experiencing compatibility issues with WiseConnect SDK 3.2.0. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s)
- Asynchronous Azure MQTT is not supported, this will be addressed in up coming release(s)
- mDNS with IPV6 is not supported
- Power Save with TCP/IP is not supported for UART interface
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer
- Recommended to use valid length(<= 202 bytes) for topic to be published in the "Embedded MQTT" demo, else it leads to return wrong error code(0x21)
- Observed AWS\_DEVICE SHADOW LOGGING STATS is not working with Power Save enable
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow
- In WPA3 transition security mode, observed sl\_wifi\_get\_wireless\_info() API is giving wrong security type and PSK in both client and AP mode
- Observed sl\_wifi\_get\_operational\_statistics() API is giving wrong opermode and wrong beacon details in AP mode
- Observed MQTT Rx is not able to resume after rejoin in wifi\_station\_ble\_provisioning\_aws demo
- In Power save, user needs to increase application stack size for Wi-Fi - AWS Device Shadow demo
- Observed issue with Wi-Fi - Powersave Deep Sleep in case of without RAM retention
- Observed issue with the firmware update over the TCP in dense environment
- Observed socket close is not working as expected for TLS socket when socket connect, send data and socket close are performing in a continuous loop
- Below are the list of applications which are not working as expected:
  - http\_otaf\_twt 
  - firmware\_flashing\_from\_host\_uart\_xmodem
- Observed UDP sendto API failure in wifi\_concurrent\_http\_server\_provisioning\_ncp application
- Observed connection issue with 3rd party AP In concurrent mode using webpage 
- Documentation not added for customized socket options
- Observed data not received from last socket while sending from multiple TCP client sockets
- WMM-PS/UAPSD is not supported

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds
- Observed connection failures with some APs in more channel congestion ~50-60% occupancy open lab environment
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) when channel congestion > 85%
- UL MU-MIMO is not supported
- While scanning with low power mode enable, sensitivity degradation of 3-6dB is observed. Due to this AP's that are at longer range might not be shown in scan results
- Passive scan is failing when DUT is configured in world domain.

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected

**WPA2 Enterprise security (STA)**

- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789"

**Wi-Fi Concurrency ( AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios
- In concurrent mode, if IP is configured for AP mode ahead of STA mode then IPv6 configuration may fail for STA mode
- In concurrent mode, data transfer using the Link-local address will always use the first IP interface created by the application

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled Low density parity check coding

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test
- Observed Performance, Interop issues with MU MIMO with certain APs
- Less throughput was observed in MU-MIMO with some APs that enabled Low density parity check coding

**MU-MIMO (UL)**

- UL MU-MIMO is not supported

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process

**IPv4/IPv6**

- IP change notification is not indicated to the application
- In concurrent mode with dual IP, if the STA starts after AP is up, the STA IP configuration may fail for DHCP stateless mode
- The example application concurrent\_firmware\_update\_from\_host\_uart has stability issues during rejoin or disconnection process
- In concurrent mode, data transfer using the Link-local address will always use the first IP interface created by the application

**BSD Socket API**

- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported
- Observing issues with TCP retries when power save mode is enabled, especially when the module is in idle state
- TCP maximum retry value is valid upto 31

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt
- Secure SSL renegotiation not supported in Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads

**SNTP**

- Unable to get SNTP async events when coex mode and power save are enabled 

**Throughputs & Performance**

- Observed 20% less Wi-Fi throughput with SDK 3.x compare to target throughput (depends on host and host interface), SDK refinements are in progress

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations
- Observed OTA failures with power save enabled, So recommended to disable power save during OTA

**Wi-Fi IOT Cloud integration** 

- **AWS IOT Core**
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
  - Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length
- **AZURE IOT Core**
  - Observed DUT after sending the data, its not sending the MQTT keep alive packet due to this Azure HUB closing the connection when power save is enabled

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with Powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency  is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 security

### **BLE**

**GAP**

- SPI interrupt miss issue was observed that prevents the host from receiving packets delivered by the TA, when there is continuous BLE TX/RX data transfer using the ble\_multiconnection\_gatt\_test application

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm and not to use power\_index 127 for BLE HP chain

**Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled
- While executing Wi-Fi Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application
- Observed BLE bonding failure during continuous HTTPS download
- Observed BLE and WLAN connection failure with SMP, when WLAN connect and HTTPS GET called in a loop
- Observed BLE disconnection during wifi commissioning with few android mobiles
- Observed data stall on the remote machine during TCP/UDP transmission with power save enabled in  wifi\_throughput\_ble\_dual\_role\_ncp example
- Observed issue in displaying throughput for interval of 'TEST\_TIMEOUT' when CONTINUOUS\_THROUGHPUT enabled in wifi\_station\_ble\_throughput\_app demo

### **Simplicity Studio and Commander (For EFR Host)**

- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain
- Universal Configurator (UC) for EFR32xG Products in NCP Mode is not supported

## **Features in RoadMap**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities
- SDIO interface support in NCP mode

### **System**

- Secure communication over host interfaces (UART, SPI)
- UART 2 Debug prints are supported only on ULP\_GPIO\_9.

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh(4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**

### **System**

- None

### **SDK**

- Baremetal mode is not supported
- WiSeConnect3\_SDK\_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade
- Lite firmware image is not supported for NCP mode.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- A maximum of 3 SSL connections are supported in Wi-Fi alone and CoEx modes.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- The maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.
- AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
- In concurrent AP mode, aggregation (Tx/Rx) is not supported.
- Embedded HTTP Server is not supported.
- mDNS with IPV6 is not supported.
- In Wi-Fi Transceiver mode, MAC level encryption/decryption is not supported. 
- Low power scan supports 1 mbps packets reception only.
- Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices
- BLE Slave latency value is valid up to 32 only
- BLE TX/RX throughput is less when tested with EFM as compared to EFR
- Maximum supported AE data length is 200 bytes
- Supports only two ADV\_EXT sets
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE
- Advertising Extension feature is not supported in Coexistence
- Isochronous channels feature is not supported
- Connection subrating feature is not supported
- LE power controller feature is not supported
- EATT feature is not supported
- Periodic Advertising with response(PAwR) feature is not supported
- BLE Audio is not supported
- The feature of dynamically changing the TX power when extended advertising is active is not supported
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address
- The maximum BLE power has been reduced by 2dB compared to the Datasheet Number, which will be addressed in the upcoming 3.3.1 patch release

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported

> **Note:** 
> 
> The following BLE Synchronous API's will be deprecated soon and the equivalent Asynchronous API's will be used instead in all BLE applications :
> 
> |**S.No**|**BLE Synchronous API's** |**BLE Asynchronous API's** |
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

# WiSeConnect3\_SDK\_3.3.0 NCP Release Notes   

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|June 18th, 2024|
|SDK Version|3\.3.0|
|Firmware Version|Standard: 1711.2.12.1.0.0.10|
|SiSDK/GSDK Version|SiSDK 2024.6.0|
|Package Name|WiSeConnect3\_SDK\_3.3.0|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components
  - Standard Wireless Firmware -  SiWx917 Firmware Binary available as SiWG917-B.2.12.1.0.0.10.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode.

> **Note:**
>
> The latest releases will have bug-fixes, enhancements, and new features in both 'SDK' and 'Firmware'. Hence, it is recommended to update and use 'SDK' and 'Firmware' of same release package.

## **Supported Hardware OPNs**

- Expansion kits: SiWx917-EB4346A (based on Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)
- IC OPNs: SiWN917M100LGTBA (Wi-Fi 6 NCP IC, QFN 7x7, 2.4 GHz, 4MB stacked flash, -40 to +85C​) 

## **Supported Features**

### **System**

- **Operating Modes** 
  - Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE, Wi-Fi Transceiver (802.11 b/g)
- **Security** 
  - Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Secure Debug. Flash Protection
  - Secure firmware upgrade options:
    - Firmware loading through UART, SPI Interface
    - Secure Over the Air (OTA) Upgrade
    - Firmware update via Bootloader
- **Crypto Support**
  - ` `Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC 
  - Wrapping Secret keys (Symmetric crypto). 
  - Added ECDSA Sign and Verify APIs.
- **System Power Save**
  - Deep Sleep with RAM retention and without RAM retention 
  - Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE

### **Wi-Fi**

- **Wi-Fi Protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- **Access Point (AP) Mode**
  - 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)
  - Wi-Fi Security  
    - WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2) 
- **Wi-Fi Scan**
  - Selective Scan, Active/Passive Scan
- **Wi-Fi STA (Security Modes)**
  - Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)
- **WPA2 Enterprise security (STA)**
  - Method
    - PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP
- **Wi-Fi STA Rejoin**
- **Wi-Fi STA Roaming** 
  - BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication
- **Wi-Fi Protocol Power Save** 
  - Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP,** TWT
- **QoS**
  - WMM-QoS
- **Wi-Fi 6 Feature**
  - MUMIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID
- **Wi-Fi Concurrency** 
  - AP+STA (Same channel)
- **Wi-Fi Band/Channels**
  - 2.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14
- **Known Security Vulnerabilities Handled**
  - WPA2 KRACK Attacks, Fragment and Forge Vulnerability

### **Network stack**

- **Core Networking Features**
  - IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6
  - SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 
  - SSL server versions TLSV1.0 and TLSV1.2
  - DHCPv4 (Client/Server)/ DHCPv6 Client
  - TCP/IP Bypass (LWIP as Hosted stack for reference)
- **Advanced Network Features**
  - HTTP Client/HTTPS Client/DNS Client/SNTP Client, Embedded MQTT/MQTT on host, IGMP
- **Wi-Fi IoT Cloud Integration**
  - AWS IoT Core
  - Azure IoT Core
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

- Simplified and Unified DX for Wi-Fi API 
- Simplifies application development and presents clean and standardized APIs
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA CoExistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth 
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.2.0 Release**

### **System**

- **Enhancements**
  - New API sl\_si91x\_fwup\_abort added to support aborting firmware upgrade
- **Fixed Issues**
  - **NA**

### **SDK**

- **Enhancements**
  - Added example application to demonstrate HTTP server provisioning and concurrent mode dual IP functionality.
  - Added support for parsing DHCP/IP notification events to Application.
  - Enhancements done in the readme and documentation.
  - Added provision to few more ciphers support in the SDK.
  - Restructured connectivity\_firmware folder with standard and lite firmwares.
  - Enhanced sl\_si91x\_set\_rtc\_time(), sl\_si91x\_host\_get\_buffer\_data() APIs with additional error handling checks.
  - Added support for parsing HTTP error codes to Application.
  - Moved eap\_flags from structure sl\_wifi\_advanced\_client\_configuration\_t to to sl\_net\_wifi\_client\_profile\_t
  - Added new API sl\_wifi\_get\_wireless\_info().
  - Updated documentation for SiSDK.
  - References to internal boards have been removed from all WSDK application readme files.
  - Enhanced socket API documentation and 'developing with sockets' guide.
  - Enhanced support for upgrading TA FW from simplicity studio launcher.
- **Fixed Issues**
  - Fixed issue with FW upgrade failure due to invalid firmware.
  - Fixed hang issue while running aws\_device\_shadow application with power save enabled.
  - Addressed the issue in the AWS MQTT demo that occurred when receiving continuous publish messages.
  - Fixed log prints issue in "Concurrent HTTP Server Provisioning" application.
  - Fixed firmware upgrade failure issue in concurrent\_firmware\_update\_from\_host\_uart demo.
  - Fixed issues in Azure connectivity issues with symmetric key authentication.
  - Fixed issue with PLL mode in coex mode.
  - Fixed issues in MQTT remote terminate handler scenarios.
  - Fixed memory leak issues while performing continuous AES encryption and decryption 
  - Fixed Readme corrections for certificate name in HTTP Client application
  - Added Fix for FW upgrade failure due to invalid firmware.
  - Added fix for 0xb44 error observed in AP mode for sl\_wifi\_get\_mac\_address API.
  - Added fix to handle proper MSS size for IPv6 in TCP & UDP. 

### **Wi-Fi/Network Stack**

- **Enhancements**
  - WLAN transmit modulation performance improvements at 85C. 
  - Minor improvements in FCC band edge performance for 11ax.
  - Added "Wi-Fi Transceiver mode" support which allows to transmit or receive 802.11 (b/g) data packets without connection.
  - Support added to trigger a passive and low power scan from host. Low power scan supports 1 mbps packets reception only.
  - Added STA-RX aggregation support in concurrent mode.
  - Added support to use user specific MAC address configured in EFUSE or flash.
  - Added feature to support Gain offset calibration in channel 14.
  - Added support to filter out multicast data packets.
- **Fixed Issues**
  - Fixed HTTP Post data 0xBBE2 error issue
  - Resolved DHCP address assignment issues in concurrent mode.
  - Enhanced the feature to skip the unicast global address assignment to stack if the static configuration is all f's
  - Fixed instability issues with concurrent mode dual IP feature.
  - Added fix for CVE-2022-23303 (WPA3 SAE side channel attack)
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
  - In the readme of the BLE and Multi protocol applications, the term 'EFR Connect App' has been replaced with 'Simplicity Connect App'.
- **Fixed Issues**
  - Updated readme for bt\_stack\_bypass demo
  - Updated readme and images for ble\_throughput\_app, ble\_secureconnection, ble\_privacy, ble\_power\_save, ble\_multiconnection\_gatt\_test, ble\_longrange\_2mbps, ble\_ibeacon, ble\_heart\_rate\_profile, ble\_central, ble\_ae\_central and ble\_ae\_peripheral demos. 
  - Added options field in "rsi\_ble\_ae\_set\_periodic\_sync" API as per specification. 

### **Multi-protocol**

- None

## **Recommendations**

### **System**

- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- Memory configuration for NCP mode is 672K\_M4SS\_0K.
- Set the following recommended FreeRTOS configuration in FreeRTOSConfig.h
  - configTIMER\_TASK\_PRIORITY to 55 
  - configTOTAL\_HEAP\_SIZE to 51200
  - configUSE\_POSIX\_ERRNO to 1

### **IDE for EFR host**

- Simplicity Studio. This release is tested with SV5.9.1.0 version.
- Recommended to use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.6.0 for Silicon labs modules.
- Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details

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
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls. 
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly.
- The minimum keep alive interval  value recommended for embedded MQTT is 10 Seconds. 
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
- In concurrent mode with dual IP, it is advised to bring up STA first (IP configuration) and AP later.
- It is recommended to enable bit 3 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host.
- It is recommended to configure Tx ,Rx , Global buffer pool ratio in the buffer config command for all Wi-Fi Socket operations from the host.

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

## **Known Issues of WiSeConnect3\_SDK\_3.3.0 Release**

### **System**

- None

### **SDK**

- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number.
- Observed not being able to receive UDP Rx TPUT print at Teraterm with IPV6 Throughput example
- Matter extension based applications are experiencing compatibility issues with WiseConnect SDK 3.2.0. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s).
- Asynchronous Azure MQTT is not supported, this will be addressed in up coming release(s).
- mDNS with IPV6 is not supported.
- Power Save with TCP/IP is not supported for UART interface.
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published in the "Embedded MQTT" demo, else it leads to return wrong error code(0x21).
- Observed AWS\_DEVICE SHADOW LOGGING STATS is not working with Power Save enable
- Observing DUT is getting stuck while running MQTT publish with 2 different topics
- Observed MQTT RX is failing when continuous data sent every 1 second in aws\_mqtt application
- Observed DUT Failed to connect to AP with timeout error 0x7(SL\_STATUS\_TIMEOUT)  while single MQTT subscribe and publish in loop with embedded MQTT application
- Observed DUT is not initiating the AWS connection post-rejoin failure indication after publishing 10-15 packets, turning OFF AP and trying to rejoin the network with aws\_mqtt application
- Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length with embedded MQTT application
- Observing MQTT disconnect API is getting failed with 0x7(SL\_STATUS\_TIMEOUT) error while doing publish & subscribe with embedded MQTT application
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow
- Observed sl\_wifi\_get\_wireless\_info() API is giving wrong security type and PSK for WPA3 Transition supported client mode.
- Observed sl\_wifi\_get\_operational\_statistics() API is giving wrong opermode and wrong beacon details in AP mode.
- Observed MQTT Rx is not able to resume after rejoin in [wifi_station_ble_provisioning_aws](https://siliconlabsconnectivitysqa.testrail.io/index.php?/tests/view/863900) demo.
- In Power save, user needs to increase application stack size for Wi-Fi - AWS Device Shadow demo.
- Observed issue with Wi-Fi - Powersave Deep Sleep in case of without RAM retention.
- Observed issue with the firmware update over the TCP in dense environment.
- Observed socket close is not working as expected for TLS socket when socket connect, send data and socket close are performing in a continuous loop.
- Below are the list of applications which are not working as expected:
  - http\_otaf\_twt 
  - firmware\_flashing\_from\_host\_uart\_xmodem

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed connection failures with some APs in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) when channel congestion > 85%.
- UL MU-MIMO is not supported.
- While scanning with low power mode enable, sensitivity degradation of 3-6dB is observed. Due to this AP's that are at longer range might not be shown in scan results.
- Observed TLS1.3 handshake failure with CHACHA20\_POLY1305\_SHA256 cipher

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional. 
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 

**WPA2 Enterprise security (STA)**

- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789"
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency ( AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled LDPC.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput was observed in MU-MIMO with some APs that enabled LDPC.

**MU-MIMO (UL)**

- UL MU-MIMO is not supported.

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process

**IPv4/IPv6**

- IP change notification is not indicated to the application
- In concurrent mode with dual IP, if the STA starts after AP is up, the STA IP configuration may fail for DHCP stateless mode.
- The example application concurrent\_firmware\_update\_from\_host\_uart has stability issues during rejoin or disconnection process.

**BSD Socket API**

- Configuration issue to program TCP retry count (default 10) through setsockopt
- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.
- Observed socket shutdown is not happening and DUT is getting stuck at  "prvCheckTasksWaitingTermination" while doing socket connection-> data transfer ->  socket shut down for multiple iterations.

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation not supported in Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads

**SNTP**

- Unable to get SNTP async events when coex mode and power save are enabled 

**Throughputs & Performance**

- Observed 20% less Wi-Fi throughput with SDK 3.x compare to target throughput (depends on host and host interface), SDK refinements are in progress

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations
- Observed OTA failures with power save enabled, So recommended to disable power save during OTA

**Wi-Fi IOT Cloud integration**

- **AWS IOT Core**
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
  - Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length
- **AZURE IOT Core**
  - Observed DUT after sending the data, its not sending the MQTT keep alive packet due to this Azure HUB closing the connection when power save is enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with Powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency  is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 security

### **BLE**

**GAP**

- SPI interrupt miss issue was observed that prevents the host from receiving packets delivered by the TA, when there is continuous BLE TX/RX data transfer using the ble\_multiconnection\_gatt\_test application. 

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm and not to use power\_index 127 for BLE HP chain.

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when Si917 device is configured as central.

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
- While executing Wi-Fi Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application
- Observed BLE bonding failure during continuous HTTPS download.
- Observed BLE and WLAN connection failure with SMP, when WLAN connect and HTTPS GET called in a loop.

### **Simplicity Studio and Commander (For EFR Host)**

- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain

## **Features in RoadMap**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities

### **System**

- Secure communication over host interfaces (UART, SPI)
- UART 2 Debug prints are supported only on ULP\_GPIO\_9.
- SiWx91x NCP connectivity FW Update Process using simplicity commander

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh(4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**

### **System**

- BRD8045A and BRD8045C adapter boards does not have software API support for hardware flow control.

### **SDK**

- Baremetal mode is not supported.
- WiSeConnect3\_SDK\_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade.
- SPI configuration for ACTIVE LOW interrupt, LOAD\_DEFAULT\_NWP\_FW\_ACTIVE\_LOW is not supported.
- Caller must memset/initialize the structure variable to 0 before calling the sl\_wifi\_get\_wireless\_info() API. This will be taken care in the next release.
- Lite firmware image is not supported for NCP mode.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- A maximum of 3 SSL connections are supported in Wi-Fi alone and CoEx modes.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- The maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.
- AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
- In concurrent AP mode, aggregation (Tx/Rx) is not supported.
- Embedded HTTP Server is not supported.
- mDNS with IPV6 is not supported.
- In Wi-Fi Transceiver mode, MAC level encryption/decryption is not supported. 
- Low power scan supports 1 mbps packets reception only.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- BLE TX/RX throughput is less when tested with EFM as compared to EFR.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported. 
- EATT feature is not supported.
- Periodic Advertising with response(PAwR) feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address.
- The maximum BLE power has been reduced by 2dB compared to the Datasheet Number, which will be addressed in the upcoming 3.3.1 patch release.

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported

> **Note:** 
> 
> The following BLE Synchronous API's will be deprecated soon and the equivalent Asynchronous API's will be used instead in all BLE applications :
> 
> |**S.No**|**BLE Synchronous API's** |**BLE Asynchronous API's** |
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

# **WiSeConnect3\_SDK\_3.2.0 NCP Release Notes**

## **Release Details**

| **Item**| **Details**|
| :- | :- |
| Release date | May 7th, 2024 |
| SDK Version | 3.2.0 |
| Firmware Version | 1711.2.11.1.0.0.7 |
| GSDK Version | 4.4.3 |
| Package Name | WiSeConnect3\_SDK\_3.2.0 |
| Supported RTOS | FreeRTOS |
| Operating Modes Supported | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP |

- SiWx917 release consists of two components
  - Wireless Firmware -  SiWx917 Firmware Binary available as SiWG917-B.2.11.1.0.0.7.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode.

## **Supported Hardware OPNs**

- Expansion kits: SiWx917-EB4346A (based on Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)
- Expansion Kits: Si917-8036B (Beta version)
- IC OPNs: SiWN917M100LGTBA (Wi-Fi 6 NCP IC, QFN 7x7, 2.4 GHz, 4MB stacked flash, -40 to +85C​)

## **Supported Features**

### **System**

- **Operating Modes**
  - Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Security**
  - Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Secure Debug. Flash Protection
  - Secure firmware upgrade options:
    - Firmware loading through UART, SPI Interface
    - Secure Over the Air (OTA) Upgrade
    - Firmware update via Bootloader
- **Crypto Support**
  -  Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC
  - Wrapping Secret keys (Symmetric crypto).
  - Added ECDSA Sign and Verify APIs.
- **System Power Save**
  - Deep Sleep with RAM retention and without RAM retention
  - Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE

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

### **Network stack**

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

- Simplified and Unified DX for Wi-Fi API
- Simplifies application development and presents clean and standardized APIs
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA CoExistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.1.4 Release**

### **System**

- None

### **SDK**

- **Enhancements**
  - Added sl\_si91x\_mem\_pool\_buffers\_with\_quota component
  - API reference guide enhancement with a callback usage note
  - Readme enhancements of the example application
- **Fixed Issues**
  - Fixed data transfer halt issues in NCP over EFR.
  - Fixed TCP RX failure in concurrent mode in multithreading\_application example
  - Image path and setup diagrams correction in example Readme
  - Handle RPS header corruption during firmware update
  - Fixed the issue of DUT returning SL\_STATUS\_TIMEOUT after multiple iterations during wifi init and deinit sequence
  - Fixed the issue where DUT is getting hang while running aws\_device\_shadow application with Power save enabled
  - Fixed handling of EAP-FAST PAC file loading manually.

### **Wi-Fi/Network Stack**

- **Enhancements**
  - Added long url support (max 2048 bytes) for HTTP OTAF.
  - Removed a limitation in Embedded where MQTT keep alive interval should be either 0 or a value greater than 35 seconds. Now allowing keep alive interval less than 35 seconds also.
  - Added support for LEAP security in EAP mode
  - Added hostname support to HTTP client request
  - For Flash less parts support added to update customer gain offset and xo\_ctune offset to external flash.
  - Added support for firmware\_update\_from\_host\_uart, wifi\_data\_transfer\_application\_ncp, wifi\_ap\_throughput\_ncp, wifi\_firmware\_flashing\_from\_host\_uart\_xmodem\_bootloader\_ncp example application.

- **Fixed Issues**
  - Fixed Embedded mqtt sequence issues when remote terminate and wlan disconnection is triggered .
  - Fixed  issues with MDNS services not responding to the queries from other devices
  - Fixed same MAC address being generated for AP and Station address incase of concurrent mode.
  - Fixed the issue where DUT is getting hang while running HTTP\_PUT Method in http client application
  - Fixed the issue where sl\_net\_set\_credential API is failed with 0x10026 (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) error when trying to load HTTP certificate(SL\_NET\_HTTP\_CLIENT\_CREDENTIAL\_ID) for HTTPs
  - Fixed issue where http otaf firmware update failed with "BBE1(SL\_STATUS\_SI91X\_HTTP\_TIMEOUT) error" for HTTP and BBD6(SL\_STATUS\_SI91X\_SSL\_TLS\_HANDSHAKE\_TIMEOUT\_OR\_FTP\_FILE\_NOT\_CLOSED) for HTTPS otaf
  - Fixed issue where DUT is not initializing TCP connection to update the firmware in HTTP OTAF (NCP)application
  - Fixed issue where failure/no response during the 2nd iteration of HTTP Server response
  - Fixed DUT hang issue which is caused because of HW calibrations.
  - Fixed xo captune calibration issue. Issue was xo captune getting overwritten with default value.
  - Fixed the issue of module not entering into deep sleep after AP stop API call.
  - Fixed an issue of AP VAP initialization in the scenario of AP-start, AP-stop and AP-start again.
  - Fixed issues with static IPV4 assignment to DUT.
  - Fixed issues with ECDSA ciphers

### **BLE**

- **Enhancements**
  - Added change to obtain 6 bytes Bluetooth device address when the "rsi\_bt\_get\_local\_device\_address" API is called in all BLE applications.
  - Added STM32 support for ble\_ae\_central, ble\_throughput\_app, ble\_power\_save and ble\_ae\_peripheral application
- **Fixed Issues**
  - Fixed BLE disconnection issue while running wifi\_https\_ble\_dual\_role application when powersave is disabled.
  - Resolved the issue with BLE PER stats cmd API.
  - Fixed reconnection issue in case of pairing failure.
  - Fixed invalid connection request(CVE-2019-19193) vulnerability.
  - Updated readme files for "ble\_accept\_list" and "ble\_testmodes" applications.
  - Updated "tx\_power" description for HP mode in "rsi-ble-per-transmit-s" structure.
  - Fixed issue where WLAN power save API is failing with error code: SL\_STATUS\_TIMEOUT while executing the ble\_ae\_central application

### **Multi-protocol**

- **Enhancements**
  - Added support for wifi\_station\_ble\_throughput\_app\_ncp, wifi\_https\_ble\_dual\_role\_ncp, wifi\_throughput\_ble\_dual\_role\_ncp example application
- **Fixed Issues**
  - Fixed BLE connection establishment when DUT in the central role while using wifi\_throughput\_ble\_dual\_role\_ncp example
  - Fixed issue where DUT is not receiving the IP address to the host even after the DHCP process over the air with the wifi\_station\_ble\_provisioning\_aws\_ncp application.
  - Fixed issue where throughputs and Remote BLE device address are not displaying in the logs for wifi\_station\_ble\_throughput\_app\_ncp example
  - Fixed issue where data transmission is not working with UART interface while running default wifi\_station\_ble\_provisioning\_aws\_uart\_ncp Application
  - Fixed issue where compilation errors are seen in the wifi\_throughput\_ble\_dual\_role\_soc example when configuring CONNECT\_OPTION  as CONN\_BY\_ADDR'
  - Fixed issue where compilation errors while Disabling HTTPS\_DOWNLOAD macro in wifi\_https\_ble\_dual\_role application.

## **Recommendations**

### **System**

- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- Memory configuration for NCP mode is 672K\_M4SS\_0K.
- Set the following recommended FreeRTOS configuration in FreeRTOSConfig.h
  - configTIMER\_TASK\_PRIORITY to 55
  - configTOTAL\_HEAP\_SIZE to 51200
  - configUSE\_POSIX\_ERRNO to 1

### **IDE for EFR host**

- Simplicity Studio. This release is tested with SV5.8.0 version.
- Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details

### **Wi-Fi/Network Stack**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode.
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications.
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative.
  - In case of TWT in coex mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration \<= 16 ms and TWT wake interval \>= 1 sec. If wake duration \> 16 ms or TWT wake interval \< 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls.
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly.
- The minimum keep alive interval  value recommended for embedded MQTT is 10 Seconds.
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

## **Known Issues of WiSeConnect3\_SDK\_3.2.0 Release**

### **System**

- None

### **SDK**

- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number.
- Observed not being able to receive UDP Rx TPUT print at Teraterm with IPV6 Throughput example
- Matter extension based applications are experiencing compatibility issues with WiseConnect SDK 3.2.0. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s).
- Asynchronous Azure MQTT is not supported, this will be addressed in up coming release(s).
- mDNS with IPV6 is not supported.
- Power Save with TCP/IP is not supported for UART interface.
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(\<= 202 bytes) for topic to be published in the "Embedded MQTT" demo, else it leads to return wrong error code(0x21).
- Observed AWS\_DEVICE SHADOW LOGGING STATS is not working with Power Save enable
- Observing DUT is getting stuck while running MQTT publish with 2 different topics
- Observed MQTT RX is failing when continuous data sent every 1 second in aws\_mqtt application
- Observed DUT Failed to connect to AP with timeout error 0x7(SL\_STATUS\_TIMEOUT)  while single MQTT subscribe and publish in loop with embedded MQTT application
- Observed DUT is not initiating the AWS connection post-rejoin failure indication after publishing 10-15 packets, turning OFF AP and trying to rejoin the network with aws\_mqtt application
- Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length with embedded MQTT application
- Observing MQTT disconnect API is getting failed with 0x7(SL\_STATUS\_TIMEOUT) error while doing publish & subscribe with embedded MQTT application
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed connection failures with some APs in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) when channel congestion \> 85%.
- UL MU-MIMO is not supported.

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional.
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected.

**WPA2 Enterprise security (STA)**

- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789"
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency ( AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios.

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled LDPC.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test.
- Observed Performance, Interop issues with MU MIMO with certain APs.
- Less throughput was observed in MU-MIMO with some APs that enabled LDPC.

**MU-MIMO (UL)**

- UL MU-MIMO is not supported.

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (\>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process

**IPv4/IPv6**

- IP change notification is not indicated to the application

**BSD Socket API**

- Configuration issue to program TCP retry count (default 10) through setsockopt
- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.
- Observed socket shutdown is not happening and DUT is getting stuck at  "prvCheckTasksWaitingTermination" while doing socket connection-\> data transfer -\>  socket shut down for multiple iterations.
- Observed DUT is not sending TCP retry packets on air when configured through setsockopt().

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation not supported in Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads
- Observed HTTPS post data is failing with error code: BBE2 (HTTP Failed).

**SNTP**

- Unable to get SNTP async events when coex mode and power save are enabled

**Throughputs & Performance**

- Observed 20% less Wi-Fi throughput with SDK 3.x compare to target throughput (depends on host and host interface), SDK refinements are in progress

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations
- Observed OTA failures with power save enabled, So recommended to disable power save during OTA
- Observed "0xffffffff" error at the remote end while doing firmware upgrade via TCP server using (featured) example with ubuntu 21.0x.

**Wi-Fi IOT Cloud integration (AWS IOT Core)**

- Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
- Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with Powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency  is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 security

### **BLE**

**GAP**

- SPI interrupt miss issue was observed that prevents the host from receiving packets delivered by the TA, when there is continuous BLE TX/RX data transfer using the ble\_multiconnection\_gatt\_test application.

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm and not to use power\_index 127 for BLE HP chain.

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when Si917 device is configured as central.

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test.
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- While executing Wi-Fi Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application

### **Simplicity Studio and Commander (For EFR Host)**

- All projects in the package are compatible with  **GNU ARM V12.2.1**  toolchain
- Project Configurator is not supported.

## **Features in RoadMap**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- Websockets
- Matter integration with Embedded TCP/IP stack
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities

### **System**

- Secure communication over host interfaces (UART, SPI, SDIO)
- UART 2 Debug prints are supported only on ULP\_GPIO\_9.
- SiWx91x NCP connectivity FW Update Process using simplicity commander

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections
- Support for BLE Mesh(4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**

### **System**

- BRD8045A and BRD8045C adapter boards does not have software API support for hardware flow control.

### **SDK**

- Baremetal mode is not supported.
- WiSeConnect3\_SDK\_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade.
- SPI configuration for ACTIVE LOW interrupt, LOAD\_DEFAULT\_NWP\_FW\_ACTIVE\_LOW is not supported

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- 40 MHz bandwidth for 2.4 GHz and 5 GHz band is not supported.
- A maximum of 3 SSL connections are supported in Wi-Fi alone and CoEx modes.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- The maximum embedded MQTT Publish payload is 1 kbyte.
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
- BLE TX/RX throughput is less when tested with EFM as compared to EFR.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- Isochronous channels feature is not supported.
- Connection subrating feature is not supported.
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with response(PAwR) feature is not supported.
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address.

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported

<br>

# **WiSeConnect3\_SDK\_3.1.4 NCP Release Notes**   

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|Mar 14th, 2024|
|SDK Version|3\.1.4|
|Firmware Version|1711\.2.10.1.3.0.7|
|Package Name|WiSeConnect3\_SDK\_3.1.4|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components
  - Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.10.1.3.0.7.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode.

## **Supported Hardware OPNs**

- Expansion kits: SiWx917-EB4346A (based on Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)
- Expansion Kits: Si917-8036B (Beta version)
- IC OPNs: SiWN917M100LGTBA (Wi-Fi 6 NCP IC, QFN 7x7, 2.4 GHz, 4MB stacked flash, -40 to +85C​) 

## **Supported Features**

### **System**

- **Operating Modes** 
  - Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Security** 
  - Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Secure Debug. Flash Protection
  - Secure firmware upgrade options:
    - Firmware loading through UART, SPI Interface
    - Secure Over the Air (OTA) Upgrade
- **Crypto Support**
  - Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC 
  - Wrapping Secret keys (Symmetric crypto).  
- **System Power Save**
  - Deep Sleep with RAM retention and without RAM retention 
  - Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE

### **Wi-Fi**

- **Wi-Fi Protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- **Access Point (AP) Mode**
  - 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha Version).
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
  - Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP, TWT
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

### **Network stack**

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

- Simplified and Unified DX for Wi-Fi API 
- Simplifies application development and presents clean and standardized APIs
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA CoExistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth 
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.1.3 Release**

### **System**
- **Enhancements**
  - None.
- **Fixed Issues**
  - None.

### **SDK**

- **Enhancements**
  - Added support for HTTP Server and provided demo application.
  - Added multithreaded support for the Concurrent mode demo(Alpha).
  - Added IPv6 support in AWS IoT MQTT Client application.
  - Added API support to get socket information from firmware.
  - Added support for featured and few snippet demos on SPI interface over STM32 (Alpha).
  - Added support for NCP over UART interface (Alpha).
  - Enhancements in throughput over SPI interface.
- **Fixed Issues**
  - Updated the missing error codes in the documentation.
  - Fixed SNTP client stop issue.

### **Wi-Fi/Network Stack**

- **Enhancements**
  - Added support to configure IPv6 link local address from the host while skipping the configuration of IPv6 unique global address for the network interface.
  - Added support for configuration of SU Beamforming capability
  - Added DNS-SD record in the MDNS query response to allow discovery of DUT's MDNS service with consistency.
- **Fixed Issues**
  - Resolved Server side dependency to repeat the firmware binary header (64 bytes).
  - Fixed MBSSID feature Interop issue with WiFi7 AP.

### **BLE**

- **Enhancements**
  - Added BLE error code details in 917 documentation
  - Added support for NCP over SPI interface using STM32 for BLE\_PER demo .
  - Added support for NCP over UART interface for few BLE demos (ble\_ae\_central, ble\_power\_save, ble\_throughput\_app,  ble\_ae\_peripheral and ble\_per.).
- **Fixed Issues**
  - Resolved the BLE hang issue when DUT was trying to connect to the nRf dongle with I/O capability as 0x00.
  - Resolved issue where DUT stopped transmitting scan request and scan response packets over air after 15 seconds while running the ble\_multiconnection\_gatt\_test application.

### **Multi-protocol**

- **Enhancements**
  - Added support for 3 SSL sockets in CoEx mode.
  - Added NCP-UART Support for Wi-Fi Client BLE Provisioning with AWS example.
- **Fixed Issues**
  - None

## **Recommendations**

### **System**

- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- Memory configuration for NCP mode is 672K\_M4SS\_0K.

### **IDE for EFR host**

- Simplicity Studio. This release is tested with SV5.8.0 version.
- Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details

### **Wi-Fi/Network Stack**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode.
- To reset TA (NWP), the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init(). 
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

### **System**

- None

### **SDK**

- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmwares older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number.
- Observed not being able to receive UDP Rx TPUT print at Teraterm with IPV6 Throughput example
- Matter extension based applications are experiencing compatibility issues with WiseConnect SDK 3.1.4. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s).
- Asynchronous Azure MQTT is not supported, this will be addressed in up coming release(s).
- Power Save with TCP/IP is not supported for UART interface.
- CLI\_DEMO and Calibration demo are not supported over UART interface.
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published in the "Embedded MQTT" demo, else it leads to return wrong error code(0x21).

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed connection failures with some APs in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) in an extremely congested environment

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional. 
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 

**WPA2 Enterprise security (STA)**

- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789"
- Configuration issue to program PAC file for EAP FAST Manual mode
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency ( AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled LDPC.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput was observed in MU-MIMO with some APs that enabled LDPC.

**MU-MIMO (UL)**

- UL MU-MIMO is not supported.

**TWT**

- Variability with MQTT keep alive interval based on TWT wake up interval.   
- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process

**IPv4/IPv6**

- Observed issue with assigning static IP to DUT
- IP change notification is not indicated to the application

**BSD Socket API**

- Configuration issue to program TCP retry count (default 10) through setsockopt
- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation not supported in Embedded Networking Stack
- observed connection failures with ECDSA ciphers

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads
- Observed HTTPS post data is failing with error code: BBE2 (HTTP Failed).Unable to get HTTP GET response when http length is exceeding 1024 bytes when SL\_SI91X\_FEAT\_LONG\_HTTP\_URL bit is disabled.
- 
**SNTP**

- Unable to get SNTP async events when coex mode and power save are enabled 

**Throughputs & Performance**

- Observed 20% less Wi-Fi throughput with SDK 3.x compare to target throughput (depends on host and host interface), SDK refinements are in progress

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations
- Observed OTA failures with power save enabled, So recommended to disable power save during OTA
- Observed "0xffffffff" error at the remote end while doing firmware upgrade via TCP server using (featured) example with ubuntu 21.0x. 

**Wi-Fi IOT Cloud integration (AWS IOT Core)**

- Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with Powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency  is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 security

### **BLE**

**GAP**

- BLE start advertising API failed with an error code 0xffffffe2(RSI\_ERROR\_RESPONSE\_TIMEOUT) within 5minutes while executing ble start ,stop and clear the address resolution API's are called in a loop continuously.
- DUT stop transmitting scan request and scan response packets over air after 15 seconds  while running the ble\_multiconnection\_gatt\_test application.
- SPI interrupt miss issue was observed that prevents the host from receiving packets delivered by the TA, when there is continuous BLE TX/RX data transfer using the ble\_multiconnection\_gatt\_test application.

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm and not to use power\_index 127 for BLE HP chain.
- BLE PER stats API returns error code: 0xFFFFFFE2 (RSI\_ERROR\_RESPONSE\_TIMEOUT) in transmit mode after sometime.

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when Si917 device is configured as central.

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- While executing Wi-Fi Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example

### **Simplicity Studio and Commander (For EFR Host)**

- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain
- Project Configurator is not supported.

## **Features in RoadMap**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- MDNS, HTTP Server, Azure MQTT Client, Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities

### **System**

- Secure communication over host interfaces (UART. SPI, SDIO)
- UART 2 Debug prints are supported only on ULP\_GPIO\_9.

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh(4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**

### **System**

- None

### **SDK**

- Baremetal mode is not supported.
- WiSeConnect3\_SDK\_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- A maximum of 3 SSL connections are supported in Wi-Fi alone and CoEx modes.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- The maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- BLE TX/RX throughput is less when tested with EFM as compared to EFR.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported. 
- EATT feature is not supported.
- Periodic Advertising with response(PAwR) feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported

<br>

# **WiSeConnect3\_SDK\_3.1.3 NCP Release Notes**   

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|Feb 14th, 2024|
|SDK Version|3\.1.3|
|Firmware Version|1711\.2.10.1.2.0.4|
|Package Name|WiSeConnect3\_SDK\_3.1.3|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components
  - Wireless Firmware -  SiWx917 Firmware Binary available as SiWG917-B.2.10.1.2.0.4.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode.

## **Supported Hardware OPNs**

- Expansion kits: SiWx917-EB4346A (based on Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)
- Expansion Kits: Si917-8036B (Beta version)
- IC OPNs: SiWN917M100LGTBA (Wi-Fi 6 NCP IC, QFN 7x7, 2.4 GHz, 4MB stacked flash, -40 to +85C​) 

## **Supported Features**

### **System**

- **Operating Modes** 
  - Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Security** 
  - Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Secure Debug. Flash Protection
  - Secure firmware upgrade options:
    - Firmware loading through UART, SPI Interface
    - Secure Over the Air (OTA) Upgrade
- **Crypto Support**
  - ` `Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC 
  - Integrated with mbed TLS 
- **System Power Save**
  - Deep Sleep with RAM retention and without RAM retention 
  - Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE

### **Wi-Fi**

- **Wi-Fi Protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- **Access Point (AP) Mode**
  - 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha Version).
  - Wi-Fi Security  
    - WPA2 Personal, WPA3 Personal (H2E method only) (Alpha Version), WPA Mixed mode (WPA/WPA2) 
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

### **Network stack**

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

- Simplified and Unified DX for Wi-Fi API 
- Simplifies application development and presents clean and standardized APIs
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA CoExistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth 
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.1.2 Release**

### **System**
- **Enhancements**
  - Enhancements done in power save module to address few stability issues
- **Fixed Issues**
  - Fixed De-init issue for the system which do not support XTAL.

### **SDK**

- **Enhancements**
  - Added mDNS feature support (Alpha Version).
  - Added Azure Freertos middleware SDK Support (Alpha Version).
  - Added "wifi_http_otaf_twt" demo.
  - Added "Wi-Fi - Azure IoT MQTT Client" demo.
  - Added support for NCP over the UART interface (Alpha version).
  - Added sl\_si91x\_set\_fast\_fw\_up() and sl\_si91x\_upgrade\_firmware() APIs for firmware upgrade.
  - Added callback support for handling TCP remote terminate event.
  - Added support for wild cards in MQTT subscription.
  - Added sl\_sntp\_client\_get\_time() API to get epoch time for SNTP.
  - Replaced licensed third party certificates with prototype certs for AWS and Azure.
  - It is recommended to enable bit 12 of the 'Feature bit map' in the opermode command for enabling long sized ICMP packet support.
  - Added support in SDK to set absolute power value in scan and join commands.
  - Added wifi6 support in PER mode for CLI demo.
  - Added "ble_multiconnection_gatt_test" demo.
  - Added "ble_unified_ae_coex_app" demo.
- **Fixed Issues**
  - In the STM32 demo applications, fixed the yield functionality issues with the priorities assigned to the other tasks vs timer task in STM32 demo applications.
  - Fixed MQTT returning an error when the incoming publish packet length is greater than 64 bytes.
  - Fixed Http client de-init hang issue when wlan disconnect occurs.
  - Fixed issues with Asynchronous socket select.
  - Updated Korea region configuration.
  - Fixed issues in OOB demo.
  - Fixed issues in standard curve parameters for SECP192R1 curve.
  - Modified RSI\_HT\_CAPS\_BIT\_MAP to support 20MHz channel width only.

### **Wi-Fi/Network Stack**

- **Enhancements**
  - Added Long URL support of 2048 bytes for HTTP Get Command.
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

- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- Memory configuration for NCP mode is 672K\_M4SS\_0K.

### **IDE for EFR host**

- Simplicity Studio. This release is tested with SV5.8.0 version.
- Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details

### **Wi-Fi/Network Stack**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode.
- To reset TA (NWP), the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init(). 
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

## **Known Issues of WiSeConnect3\_SDK\_3.1.3 Release**

### **System**

- None

### **SDK**

- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmwares older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number.
- Observed not being able to receive UDP Rx TPUT print at Teraterm with IPV6 Throughput example
- Matter applications are experiencing compatibility issues with WiseConnect SDK 3.1.2. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s).
- Asynchronous Azure MQTT is not supported, this will be addressed in up coming release(s).
- Disable power save before making TCP connection for UART interface.

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed connection failures with some APs in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) in an extremely congested environment

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional. 
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 

**WPA2 Enterprise security (STA)**

- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789"
- Configuration issue to program PAC file for EAP FAST Manual mode
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency ( AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled LDPC.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput was observed in MU-MIMO with some APs that enabled LDPC.

**MU-MIMO (UL)**

- UL MU-MIMO is not supported.

**TWT**

- Variability with MQTT keep alive interval based on TWT wake up interval.   
- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process

**IPv4/IPv6**

- Observed issue with assigning static IP to DUT
- IP change notification is not indicated to the application

**BSD Socket API**

- Configuration issue to program TCP retry count (default 10) through setsockopt
- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation not supported in Embedded Networking Stack
- observed connection failures with ECDSA ciphers

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads
- Observed HTTPS post data is failing with error code: BBE2 (HTTP Failed).Unable to get HTTP GET response when http length is exceeding 1024 bytes when SL\_SI91X\_FEAT\_LONG\_HTTP\_URL bit is disabled.
- 
**SNTP**

- Unable to get SNTP async events when coex mode and power save are enabled 

**Throughputs & Performance**

- Observed 20% less Wi-Fi throughput with SDK 3.x compare to target throughput (depends on host and host interface), SDK refinements are in progress

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations
- Observed OTA failures with power save enabled, So recommended to disable power save during OTA
- Observed "0xffffffff" error at the remote end while doing firmware upgrade via TCP server using (featured) example with ubuntu 21.0x. 

**Wi-Fi IOT Cloud integration (AWS IOT Core)**

- Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with Powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency  is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 security

### **BLE**

**GAP**

- BLE start advertising API failed with an error code 0xffffffe2(RSI\_ERROR\_RESPONSE\_TIMEOUT) within 5minutes while executing ble start ,stop and clear the address resolution API's are called in a loop continuously.
- DUT stop transmitting scan request and scan response packets over air after 15 seconds  while running the ble_multiconnection_gatt_test application.
- SPI interrupt miss issue was observed that prevents the host from receiving packets delivered by the TA, when there is continuous BLE TX/RX data transfer using the ble_multiconnection_gatt_test application.

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm and not to use power\_index 127 for BLE HP chain.

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when Si917 device is configured as central.

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- While executing Wi-Fi Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example

### **Simplicity Studio and Commander (For EFR Host)**

- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain
- Project Configurator is not supported.

## **Features in RoadMap**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- MDNS, HTTP Server, Azure MQTT Client, Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities

### **System**

- Secure communication over host interfaces (UART. SPI, SDIO)
- UART 2 Debug prints are supported only on ULP\_GPIO\_9.

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh(4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**

### **System**

- None

### **SDK**

- Baremetal mode is not supported.
- WiSeConnect3\_SDK\_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- A maximum of 3 SSL connections are supported in Wi-Fi alone mode.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- The maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- BLE TX/RX throughput is less when tested with EFM as compared to EFR.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported. 
- EATT feature is not supported.
- Periodic Advertising with response(PAwR) feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported

<br>

# **WiSeConnect3\_SDK\_3.1.2 NCP Release Notes**   

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|Jan 17th, 2024|
|SDK Version|3\.1.2|
|Firmware Version|1711\.2.10.1.1.0.2|
|Package Name|WiSeConnect3\_SDK\_3.1.2|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components
  - Wireless Firmware -  SiWx917 Firmware Binary available as SiWG917-B.2.10.1.1.0.2.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode.

## **Supported Hardware OPNs**

- Expansion kits: SiWx917-EB4346A (based on Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)
- Expansion Kits: Si917-8036B (Beta version)
- IC OPNs: SiWN917M100LGTBA (Wi-Fi 6 NCP IC, QFN 7x7, 2.4 GHz, 4MB stacked flash, -40 to +85C​) 

## **Supported Features**

### **System**

- **Operating Modes** 
  - Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Security** 
  - Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Secure Debug. Flash Protection
  - Secure firmware upgrade options:
    - Firmware loading through UART, SPI Interface
    - Secure Over the Air (OTA) Upgrade
- **Crypto Support**
  - ` `Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC 
  - Integrated with mbed TLS 
- **System Power Save**
  - Deep Sleep with RAM retention and without RAM retention 
  - Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE

### **Wi-Fi**

- **Wi-Fi Protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- **Access Point (AP) Mode**
  - 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode
  - Wi-Fi Security  
    - WPA2 Personal, WPA3 Personal (H2E method only), WPA Mixed mode (WPA/WPA2) 
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

### **Network stack**

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

- Simplified and Unified DX for Wi-Fi API 
- Simplifies application development and presents clean and standardized APIs
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA CoExistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth 
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.1.1-Doc Release**

### **System**

- WOW LAN is not supported.

### **SDK**

- **Enhancements**
  - Resolved warnings in the featured and snippet demos.
  - sl\_wifi\_get\_sta\_tsf() API added to get the station TSF time which is synchronized with connected AP beacon TSF.
  - Added "update gain” demo for BLE.
  - Added an option to test user gain table feature in "ble\_per" demo by enabling the “GAIN\_TABLE\_AND\_MAX\_POWER\_UPDATE\_ENABLE” macro.
  - Removed unused input parameters in rsi\_ble\_set\_ble\_tx\_power() API.
- **Fixed Issues**
  - Fixed issue with BSSID based connection.
  - Fixed issue with the file descriptors set handling for socket select.
  - Fixed issue with authentication and association timeout is not reflecting as per timeout configuration using sl\_si91x\_configure\_timeout() API
  - Fixed profile id corruption issue for IPV6 demos. 
  - Fixed application hang issue at the AE Clear Periodic Adv list API when used "ble\_ae\_central" example. 
  - Fixed issue where BLE connect API is failing because of 0xFFFFFFE2(RSI\_ERROR\_RESPONSE\_TIMEOUT) error with ble\_central application.

### **Wi-Fi/Network Stack**

- **Enhancements**
  - Added support for TSF time which is synchronized with connected AP beacon TSF.
  - Improve PER performance at max input power levels for 11b.
  - Improved ppm error across channels at room temperature.
  - Updated regulatory tables of WLAN
- **Fixed Issues**
  - Increased supported length for Embedded MQTT username up to 120 bytes and topic length up to 200 bytes.
  - Added support to process long ICMP ping response up to packet size of 1472 Bytes for IPv4 and 1452 Bytes for IPv6.
  - Issue with configuration of TCP MSS through setsockopt() has been fixed.
  - Fixed Listen interval based power save issue when IPV6 mode is enabled
  - Added fix for missing RSN IE in first few AP beacons

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

- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- Memory configuration for NCP mode is 672K\_M4SS\_0K.

### **IDE for EFR host**

- Simplicity Studio. This release is tested with SV5.8.0 version.
- Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details

### **Wi-Fi/Network Stack**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode.
- To reset TA (NWP), the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init(). 
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

## **Known Issues of WiSeConnect3\_SDK\_3.1.2 Release**

### **System**

- None

### **SDK**

- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmwares older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number.
- Observed not being able to receive UDP Rx TPUT print at Teraterm with IPV6 Throughput example
- Matter applications are experiencing compatibility issues with WiseConnect SDK 3.1.2. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s).

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed connection failures with some APs in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) in an extremely congested environment
- Note section in API documentation for sl\_wifi\_get\_sta\_tsf  needs to be corrected to "Returns error if station is not connected or at least one beacon is not received."

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional. 
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 

**WPA2 Enterprise security (STA)**

- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789"
- Configuration issue to program PAC file for EAP FAST Manual mode
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency ( AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled LDPC.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput was observed in MU-MIMO with some APs that enabled LDPC.

**TWT**

- Variability with MQTT keep alive interval based on TWT wake up interval.   
- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process

**IPv4/IPv6**

- Observed issue with assigning static IP to DUT
- IP change notification is not indicated to the application

**BSD Socket API**

- Configuration issue to program TCP retry count (default 10) through setsockopt
- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation not supported in Embedded Networking Stack
- observed connection failures with ECDSA ciphers

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads
- Observed HTTPS post data is failing with error code: BBE2 (HTTP Failed).

**SNTP**

- Unable to get SNTP async events when coex mode and power save are enabled 

**Throughputs & Performance**

- Observed 20% less Wi-Fi throughput with SDK 3.x compare to target throughput (depends on host and host interface), SDK refinements are in progress

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations
- Observed OTA failures with power save enabled, So recommended to disable power save during OTA
- Observed "0xffffffff" error at the remote end while doing firmware upgrade via TCP server using (featured) example with ubuntu 21.0x. 

**Wi-Fi IOT Cloud integration (AWS IOT Core)**

- Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with Powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency  is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 security

### **BLE**

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when Si917 device is configured as central.

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- While executing Wi-Fi Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example

### **Simplicity Studio and Commander (For EFR Host)**

- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain
- Project Configurator is not supported.

## **Features in RoadMap**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- MDNS, HTTP Server, Azure MQTT Client, Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities

### **System**

- Secure communication over host interfaces (UART, SPI, SDIO)
- UART 2 Debug prints are supported only on ULP\_GPIO\_9.

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh(4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**

### **System**

- None

### **SDK**

- Baremetal mode is not supported.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- A maximum of 3 SSL connections are supported in Wi-Fi alone mode.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- The maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- BLE TX/RX throughput is less when tested with EFM as compared to EFR.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported. 
- EATT feature is not supported.
- Periodic Advertising with response(PAwR) feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported

<br>

# **WiSeConnect3\_SDK\_3.1.1-Hotfix NCP Release Notes**

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|Jan 2nd, 2024|
|SDK Version|3\.1.1-Hotfix|
|Firmware Version|1711\.2.10.1.0.0.7|
|Package Name|WiSeConnect3\_SDK\_3.1.1-Hotfix|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components
  - Wireless Firmware -  SiWx917 Firmware Binary available as SiWG917-B.2.10.1.0.0.x.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode.

## **Supported Hardware OPNs**

- Expansion kits: SiWx917-EB4346A (based on Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)
- Expansion Kits: Si917-8036B (Beta version)
- IC OPNs: SiWN917M100LGTBA (Wi-Fi 6 NCP IC, QFN 7x7, 2.4 GHz, 4MB stacked flash, -40 to +85C​) 

## **Supported Features**

### **System**

- **Operating Modes** 
  - Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Security** 
  - Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Secure Debug. Flash Protection
  - Secure firmware upgrade options:
    - Firmware loading through UART, SPI Interface
    - Secure Over the Air (OTA) Upgrade
- **Crypto Support**
  - Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC 
  - Integrated with mbed TLS 
- **System Power Save**
  - Deep Sleep with RAM retention and without RAM retention 
  - Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE

### **Wi-Fi**

- **Wi-Fi Protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- **Access Point (AP) Mode**
  - 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode
  - Wi-Fi Security  
    - WPA2 Personal, WPA3 Personal (H2E method only), WPA Mixed mode (WPA/WPA2) 
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

### **Network stack**

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

- Simplified and Unified DX for Wi-Fi API 
- Simplifies application development and presents clean and standardized APIs
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA CoExistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth 
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.1.1-Doc Release**

### **System**

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

- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- Memory configuration for NCP mode is 672K\_M4SS\_0K.

### **IDE for EFR host**

- Simplicity Studio. This release is tested with SV5.8.0 version.
- Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details

### **Wi-Fi/Network Stack**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode.
- To reset TA (NWP), the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init(). 
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

### **System**

- None

### **SDK**

- Observed issue, when NULL is passed for readfds, writefds, excepfds to socket, select. 
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmwares older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number.
- Observed not being able to receive UDP Rx TPUT print at Teraterm with IPV6 Throughput example

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed BSSID-based connection configuration is not reflecting to make a connection with the specified access point.
- Observed connection failures with some APs in more channel congestion ~50-60% occupancy open lab environment.
- Observed auth/assoc timeout is not reflecting as per timeout configuration using sl\_si91x\_configure\_timeout() API .
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) in an extremely congested environment

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional. 
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 
- First few beacons (~ 10) don't include RSN IE

**WPA2 Enterprise security (STA)**

- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789"
- Configuration issue to program PAC file for EAP FAST Manual mode
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency ( AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled LDPC.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput was observed in MU-MIMO with some APs that enabled LDPC.

**TWT**

- Variability with MQTT keep alive interval based on TWT wake up interval.   
- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process

**IPv4/IPv6**

- Observed issue with assigning static IP to DUT
- IP change notification is not indicated to the application
- Observed sl\_net\_up returning 0x27 error code due to an issue in the handling of IPv6 address. 

**BSD Socket API**

- Configuration issue to program MSS (default 1460), TCP retry count (default 10) through setsockopt
- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation not supported in Embedded Networking Stack
- observed connection failures with ECDSA ciphers

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads
- Observed HTTPS post data is failing with error code: BBE2 (HTTP Failed).

**SNTP**

- Unable to get SNTP async events when coex mode and power save are enabled 

**Throughputs & Performance**

- Observed 20% less Wi-Fi throughput with SDK 3.x compare to target throughput (depends on host and host interface), SDK refinements are in progress

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations
- Observed OTA failures with power save enabled, So recommended to disable power save during OTA
- Observed "0xffffffff" error at the remote end while doing firmware upgrade via TCP server using (featured) example with ubuntu 21.0x. 

**Wi-Fi IOT Cloud integration (AWS IOT Core)**

- Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with Powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency  is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 security

### **BLE**

**GAP**

- When running the ble\_central application with power save enabled, it was observed that the BLE connect API is failing because of 0xFFFFFFE2(RSI\_ERROR\_RESPONSE\_TIMEOUT) error.
- When BLE local name is provided with a 16-byte length, the Set local name API fails with error code: 0x4e66[Invalid Name length].

**Advertising Extension**

- When running the ble\_ae\_central example, observed an application hang issue at the AE Clear Periodic Adv list API. 

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 16 dBm.  Please don't use for 127 power\_index for BLE HP release with this release.

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when Si917 device is configured as central.

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- While executing Wi-Fi Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example

### **Simplicity Studio and Commander (For EFR Host)**

- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain
- Project Configurator is not supported.

## **Features in RoadMap**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- MDNS, HTTP Server, Azure MQTT Client, Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities

### **System**

- Secure communication over host interfaces (UART. SPI, SDIO)
- UART 2 Debug prints are supported only on ULP\_GPIO\_9.

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh(4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**

### **System**

- None

### **SDK**

- Baremetal mode is not supported.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- A maximum of 3 SSL connections are supported in Wi-Fi alone mode.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- The maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- BLE TX/RX throughput is less when tested with EFM as compared to EFR.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported. 
- EATT feature is not supported.
- Periodic Advertising with response(PAwR) feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported

<br>

# **WiSeConnect3\_SDK\_3.1.1-Doc NCP Release Notes**

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|Dec 20th, 2023|
|SDK Version|3\.1.1-Doc|
|Firmware Version|1711\.2.10.1.0.0.4|
|Package Name|WiSeConnect3\_SDK\_3.1.1-Doc|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components
  - Wireless Firmware -  SiWx917 Firmware Binary available as SiWG917-B.2.10.0.0.4.x.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode.

## **Supported Hardware OPNs**

- Expansion kits: SiWx917-EB4346A (based on Radio board SiWx917-4346A + 8045A Co-Processor Adapter board)
- Expansion Kits: Si917-8036B (Beta version)
- IC OPNs: SiWN917M100LGTBA (Wi-Fi 6 NCP IC, QFN 7x7, 2.4 GHz, 4MB stacked flash, -40 to +85C​) 

## **Supported Features**

### **System**

- **Operating Modes** 
  - Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Security** 
  - Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Secure Debug. Flash Protection
  - Secure firmware upgrade options:
    - Firmware loading through UART, SPI Interface
    - Secure Over the Air (OTA) Upgrade
- **Crypto Support**
  - Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC 
  - Integrated with mbed TLS 
- **System Power Save**
  - Deep Sleep with RAM retention and without RAM retention 
  - Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE

### **Wi-Fi**

- **Wi-Fi Protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- **Access Point (AP) Mode**
  - 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode
  - Wi-Fi Security  
    - WPA2 Personal, WPA3 Personal (H2E method only), WPA Mixed mode (WPA/WPA2) 
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

### **Network stack**

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

- Simplified and Unified DX for Wi-Fi API 
- Simplifies application development and presents clean and standardized APIs
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA CoExistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth 
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.1.0 Release**

### **System**

- None

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
  - Added support for customization support of ncp\_host and spi\_driver implementations using efx\_ncp\_custom\_host\_mcu and siwx917\_ncp\_custom\_spi\_driver respectively.
  - Updated readme files for AES, calibration app, HTTP client, WLAN throughput v6, and embedded MQTT client.
  - Updated documentation for the CLI demo in NCP mode.
  - added readme files for ble\_per and ble\_accept\_list.
  - Updated BLE API reference documentation with descriptions and navigation links.
  - Replaced printf statements with LOG\_PRINT in all Coex example applications.

### **Wi-Fi/Network Stack**

- **Enhancements**
  - Added support for WPA3 personal security (with H2E method only) in AP mode
  - Added support for timeout configuration API 
  - Added support for Server Name Indication (SNI).
  - Added support for Extensible Authentication Protocol (EAP) encrypted certificates.
  - Added support for CLI demo, HTTP client, WLAN throughput v6, and embedded MQTT client example applications for NCP mode
- **Fixed Issues**
  - Resolved issues with CW modes in RF test example.
  - Fixed the issue where application hangs  during SHA operation on larger data exceeding 1400 bytes in TA(NWP).
  - Fixed the issue where the IOT socket connect/bind failed with -3 error in the CLI demo app.
  - Fixed the issue where DUT BLE stopped advertising after a few seconds while running wifi\_ble\_power\_save\_ncp application.
  - Fixed the issue where sl\_net\_init returned 0xff82 as an error code in the case of WLAN throughput NCP.

### **BLE**

- **Added NCP Support For**
  - ble\_power\_save, gatt\_long\_read, ble\_hid\_on\_gatt, ble\_unified\_ae\_coex\_app, ble\_datalength, ble\_accept\_list, ble\_central, ble\_longrange\_2mpbps, ble\_heart\_rate\_profile, ble\_ibeacon, ble\_privacy, ble\_secureconnection, ble\_throughput\_app, ble\_ae\_central and ble\_ae\_peripheral applications.
- **Enhancements**
  - Added support for standby sleep with RAM retention in WLAN+BLE coex mode.
  - Removed support for unused rsi\_ble\_set\_prop\_protocol\_ble\_bandedge\_tx\_power(), rsi\_bt\_ber\_enable\_or\_disable(), and rsi\_bt\_per\_cw\_mode() APIs.
  - Added support for rsi\_ble\_prepare\_write\_async, rsi\_ble\_execute\_write\_async and rsi\_ble\_get\_inc\_services SAPI's in gatt\_test application.
- **Fixed Issues**
  - DUT transmitting 240 bytes MTU request packet regardless of programmed MTU length.
  - DUT is unable to transmit complete periodic advertising data.
  - Hang issue when transmitting BLE notification with coded PHY rates of 125kbps and 500kbps.
  - DUT not transmitting BLE scan requests after 20sec when power save is enabled.
  - DUT not transmitting packets continuously over the air in BLE test modes.
  - BLE Set BD address API failing with error code 0xfffffffd (Command given in the wrong state).
  - rsi\_ble\_set\_local\_att\_value() API fails with error code 0xFFFFFFE2 (RSI\_ERROR\_RESPONSE\_TIMEOUT) during BLE notification.
  - "Failed to keep module in active mode: FFFFFFFF" issue when disabling power save after enabling enhanced max psp power save.
  - BLE per stats cmd API fails with error code 0xFFFFFFE2 (RSI\_ERROR\_RESPONSE\_TIMEOUT).
  - BLE scanning stops within ~4-5 minutes when powersave is enabled.

### **Multi-protocol**

- **Enhancements**
  - Added support for wifi\_station\_ble\_provisioning with AWS cloud.
  - Increased priority of Wi-Fi handling of data transmission in coex mode.
- **Fixed Issues**
  - Abrupt termination of HTTP download in the application when Powersave is enabled.
  - Firmware update fails with 0x7 error code when power save is enabled in wifi\_throughput\_ble\_dual\_role application.
  - Handling of PLL value in coex mode.
  - BLE disconnection issue after ~25-30 min while running WLAN HTTP/HTTPs download along with BLE data transfer with Power save enabled.

## **Recommendations**

### **System**

- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- Memory configuration for NCP mode is 672K\_M4SS\_0K.

### **IDE for EFR host**

- Simplicity Studio. This release is tested with SV5.8.0 version.
- Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details

### **Wi-Fi/Network Stack**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode.
- To reset TA (NWP), the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init(). 
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

### **System**

- None

### **SDK**

- Observed issue, when NULL is passed for readfds, writefds, excepfds to socket, select. 
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmwares older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number.
- Observed not being able to receive UDP Rx TPUT print at Teraterm with IPV6 Throughput example

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed BSSID-based connection configuration is not reflecting to make a connection with the specified access point.
- Observed connection failures with some APs in more channel congestion ~50-60% occupancy open lab environment.
- Observed auth/assoc timeout is not reflecting as per timeout configuration using sl\_si91x\_configure\_timeout() API .
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) in an extremely congested environment

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional. 
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 
- First few beacons (~ 10) don't include RSN IE

**WPA2 Enterprise security (STA)**

- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789"
- Configuration issue to program PAC file for EAP FAST Manual mode
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency ( AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled LDPC.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput was observed in MU-MIMO with some APs that enabled LDPC.

**TWT**

- Variability with MQTT keep alive interval based on TWT wake up interval.   
- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process

**IPv4/IPv6**

- Observed issue with assigning static IP to DUT
- IP change notification is not indicated to the application
- Observed sl\_net\_up returning 0x27 error code due to an issue in the handling of IPv6 address. 

**BSD Socket API**

- Configuration issue to program MSS (default 1460), TCP retry count (default 10) through setsockopt
- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation not supported in Embedded Networking Stack
- observed connection failures with ECDSA ciphers

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads
- Observed HTTPS post data is failing with error code: BBE2 (HTTP Failed).

**SNTP**

- Unable to get SNTP async events when coex mode and power save are enabled 

**Throughputs & Performance**

- Observed 20% less Wi-Fi throughput with SDK 3.x compare to target throughput (depends on host and host interface), SDK refinements are in progress

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations
- Observed OTA failures with power save enabled, So recommended to disable power save during OTA
- Observed "0xffffffff" error at the remote end while doing firmware upgrade via TCP server using (featured) example with ubuntu 21.0x. 

**Wi-Fi IOT Cloud integration (AWS IOT Core)**

- Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with Powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency  is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 security

### **BLE**

**GAP**

- When running the ble\_central application with power save enabled, it was observed that the BLE connect API is failing because of 0xFFFFFFE2(RSI\_ERROR\_RESPONSE\_TIMEOUT) error.
- When BLE local name is provided with a 16-byte length, the Set local name API fails with error code: 0x4e66[Invalid Name length].

**Advertising Extension**

- When running the ble\_ae\_central example, observed an application hang issue at the AE Clear Periodic Adv list API. 

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 16 dBm.  Please don't use for 127 power\_index for BLE HP release with this release.

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when Si917 device is configured as central.

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- While executing Wi-Fi Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example

### **Simplicity Studio and Commander (For EFR Host)**

- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain
- Project Configurator is not supported.

## **Features in RoadMap**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- MDNS, HTTP Server, Azure MQTT Client, Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities

### **System**

- Secure communication over host interfaces (UART. SPI, SDIO)
- UART 2 Debug prints are supported only on ULP\_GPIO\_9.

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh(4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**

### **System**

- None

### **SDK**

- Baremetal mode is not supported.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- A maximum of 3 SSL connections are supported in Wi-Fi alone mode.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- The maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- BLE TX/RX throughput is less when tested with EFM as compared to EFR.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported. 
- EATT feature is not supported.
- Periodic Advertising with response(PAwR) feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported

<br>

# WiSeConnect3\_SDK\_3.1.1 NCP Release Notes
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
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode 
- This release is meant only for use with designs based on SiWx917 Silicon
- The firmware to load in your design are available in the following format:
  - SiWG917-B.2.10.0.0.4.x.rps -  Sutlej B0 Full-featured firmware binary
## Supported hardware/device setups
- NCP EFR Expansion Kit with NCP Radio board SiWx917-EB4346A (BRD4346A + BRD8045A)
- BRD4002A Wireless pro kit mainboard SI-MB4002A
- EFR32xG24 Wireless 2.4 GHz +10 dBm Radio Board xG24-RB4186C
## Features Supported
### **System**
- #### **Operating Modes :**
  - Wi-Fi STA (802.11ax, 802.11n),
  - Wi-Fi 802.11n AP,
  - Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP,
  - Wi-Fi STA (802.11ax, 802.11n) + BLE:
- #### **Host Interface :**
  - SPI
- #### **Security** 
  - Secure Boot
  - Secure firmware upgrade over
    - ISP
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
- #### **Power save**
  - Deep Sleep 
    - With RAM retention
    - Without RAM retention
  - Connected Sleep
  - Host based wake-up
### **Wi-Fi**
- #### **WiFi protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- #### **Access Point (AP) mode**
  - 4 Client Support 
  - WiFi Security  
    - WPA2 Personal
    - WPA3 Personal (H2E method only)
    - WPA Mixed mode (WPA/WPA2) 
  - Hidden SSID Mode
  - Auto Channel Selection
  - Scan in AP mode
- #### **Wi-Fi Scan**
  - Selective Scan
  - Active/Passive Scanning
- #### **Wi-Fi STA (Security Modes)**
  - Open Mode
  - WPA2 Personal, WPA2 Enhancements
  - WPA3 Personal
  - Mixed mode (WPA/WPA2)
  - WPA3 Personal Transition Mode (WPA2/WPA3)
- #### **WPA2 Enterprise security**
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
  - iTWT,** TWT I-Frame & TWT Enhancements
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
- GAP ( Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol (GATT)
- Attribute protocol (ATT)
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
- Simultaneous scanning on 1M and Coded PHY
- LE dual role topology
- LE data packet length extensions( DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE secure connections
- Bluetooth 5.4 Qualified( Clerical work pending)
### **Multi-protocol Mode**
- Wi-Fi STA + BLE
### **PTA CoExistence**
- 3 wire coex acting as Wi-Fi with external Bluetooth 
- 3 wire coex acting as Wi-Fi with external Zigbee/OT
## Changes and Fixes
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
  - Updated and standardized README files for featured examples, MQTT, TWT, and more.
  - Generic API reference documentation updation.
  - Added support for CLI demo, HTTP client, WLAN throughput v6, and embedded MQTT client example applications for NCP mode
  - Added support for customization support of ncp\_host and spi\_driver implementations using efx\_ncp\_custom\_host\_mcu and siwx917\_ncp\_custom\_spi\_driver respectively.
  - Updated readme files for AES, calibration app, HTTP client, WLAN throughput v6, and embedded MQTT client.
  - Updated documentation for the CLI demo in NCP mode.
- #### **Fixed Issues**
  - Resolved issues with CW modes in RF test example.
  - Fixed the issue where application hangs issue during SHA operation on larger data exceeding 1400 bytes in TA(NWP).
  - Fixed the issue where DUT sent non-aggregated packets with protected ADDBA response for improving throughput.
  - Fixed the issue where the IOT socket connect/bind failed with -3 error in the CLI demo app.
  - Fixed the issue where DUT\_BLE stopped advertising after a few seconds while running wifi\_ble\_power\_save\_ncp application.
  - Fixed the issue where sl\_net\_init returned 0xff82 as an error code in the case of WLAN throughput NCP.
### **BLE**
- #### **Added NCP support for**
  - ble\_power\_save
  - gatt\_long\_read
  - ble\_hid\_on\_gatt
  - ble\_unified\_ae\_coex\_app
  - ble\_datalength
  - ble\_accept\_list
  - ble\_central
  - ble\_longrange\_2mpbps
  - ble\_heart\_rate\_profile
  - ble\_ibeacon
  - ble\_privacy
  - ble\_secureconnection
  - ble\_throughput\_app
  - ble\_ae\_central and ble\_ae\_peripheral applications.
- #### **Enhancements**
  - Readme files for ble\_per and ble\_accept\_list.
  - BLE API reference documentation with descriptions and navigation links.
  - Added support for standby sleep with RAM retention in WLAN+BLE coex mode.
  - Removed support for unused rsi\_ble\_set\_prop\_protocol\_ble\_bandedge\_tx\_power(), rsi\_bt\_ber\_enable\_or\_disable(), and rsi\_bt\_per\_cw\_mode() APIs.
  - Added support for rsi\_ble\_prepare\_write\_async, rsi\_ble\_execute\_write\_async and rsi\_ble\_get\_inc\_services SAPI's in gatt\_test application.
- #### **Fixed issues**
  - DUT transmitting 240 bytes MTU request packet regardless of programmed MTU length.
  - DUT is unable to transmit complete periodic advertising data.
  - Hang issue when transmitting BLE notification with coded PHY rates of 125kbps and 500kbps.
  - DUT not transmitting BLE scan requests after 20sec when power save is enabled.
  - DUT not transmitting packets continuously over the air in BLE test modes.
  - BLE Set BD address API failing with error code 0xfffffffd (Command given in the wrong state).
  - rsi\_ble\_set\_local\_att\_value() API fails with error code 0xFFFFFFE2 (RSI\_ERROR\_RESPONSE\_TIMEOUT) during BLE notification.
  - "Failed to keep module in active mode: FFFFFFFF" issue when disabling power save after enabling enhanced max psp power save.
  - BLE per stats cmd API fails with error code 0xFFFFFFE2 (RSI\_ERROR\_RESPONSE\_TIMEOUT).
  - BLE scanning stops within ~4-5 minutes when powersave is enabled.
### **Mutli-protocol**
- #### **Enhancements**
  - Added support for wifi\_station\_ble\_provisioning with AWS cloud.
  - Added support for the timer (with the power save disabled) in wifi\_station\_ble\_provisioning\_aws example.
  - Added support for alarm-based power save mode in wlan\_station\_ble\_provisioning\_with\_aws cloud example application.
  - Increased priority of Wi-Fi handling of data transmission in coex mode.
  - Replaced Printf statements with LOG\_PRINT in all Coex example applications.
- #### **Fixed issues**
  - Abrupt termination of HTTP download in the application when Powersave is enabled.
  - Firmware update fails with 0x7 error code when power save is enabled in wifi\_throughput\_ble\_dual\_role application.
  - Handling of PLL value in coex mode.
  - BLE disconnection issue after ~25-30 min while running WLAN HTTP/HTTPs download along with BLE data transfer with Power save enabled.
## Recommendations
### **System**
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- Memory configuration for NCP mode is 672K\_M4SS\_0K.
### **IDE**
- Simplicity Studio. This release is tested with SV5.8.0 version.
- Refer to the latest version of the NCP "Getting-Started-with-SiWx917" guide for more details
### **Wi-Fi/Network Stack**
- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- Aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode. This enables software to aggregate multiple packets to achieve better throughput
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
- Secure SSL renegotiation not supported in Embedded Networking Stack
- Every server socket created consumes a socket and every subsequent connection consumes an additional socket.
- Observed failure with BIT\_TLS\_ECDHE\_ECDSA\_WITH\_AES\_256\_CBC\_SHA384, BIT\_TLS\_ECDHE\_ECDSA\_WITH\_AES\_128\_CBC\_SHA256 ciphers
- Configuration issue to program MSS, TCP retry count 
- Observed inconsistency with MQTT keep alive when TWT is enabled
- TWT device (STA) may not go to sleep in between TWT intervals in case of burst data traffic where data transfer happens over multiple TWT service periods.
- When sl\_wifi\_enable\_target\_wake\_time() API is used, observed occasional MQTT disconnections with longer TWT intervals (>30secs) with embedded MQTT + TWT.
- There are performance and IOP issues with MU MIMO.
- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
- Observed HTTPS Continuous download fails with TA Power save
- Planned performance improvements are in progress to get optimal performance in Wi-Fi/WiFi+BLE 
- Observed interop issue (connectivity failure) with few AP's(EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID) when device reset/reboot happens on active connection. 
- Unable to get SNTP async events when coex mode and power save are enabled and not getting any response for sl\_net\_up API for 2nd Iteration
- Disconnections observed with Netgear RAX120 AP in WPA3 security
- Observed connection failures with some AP's in more channel congestion ~50-60% occupancy open lab environment. 
- In AP mode, first few beacons don't include RSN IE.
- Issue with fixed rate configuration in AP mode.
- Configuration issue to program PAC file for EAP FAST Manual  mode.
- Optional LDPC feature is not enabled, So observed less throughput in DL-OFDMA with some APs.
- Observed issue with configuring Certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observed auth/assoc timeout is not affecting/working as per timeout configuration.
- Observed sl\_net\_deinit is not returning any response for both "SL\_NET\_WIFI\_AP\_INTERFACE" and "SL\_NET\_WIFI\_CLIENT\_INTERFACE" interfaces.
- Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
- WoWLAN feature not supported.
- Observed QOS1 publish is not happening in 3rd iteration while aws\_mqtt application running in loop
- Observed DUT is going back to power save before the monitor interval when there is no RX data from AP in ENH MAX PSP
- Observed DUT is giving Incorrect error code(0x5) for SNTP create when ludp socket is opened at 123 port.
### **BLE**
- When running the ble\_central application with power save enabled, it was observed that the BLE connect API is failing because of 0xFFFFFFE2(RSI\_ERROR\_RESPONSE\_TIMEOUT) error.
- When running the ble\_ae\_central example, observed an application hang issue at the AE Clear Periodic Adv list API. 
- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when device is configured as central.
- The maximum power that BLE can support is 16 dBm. It is not permitted for users to program more than 16dBm. 127 power_index is not supported in this release.
### **Multi-protocol**
- Observed inconsistency issues with Wi-Fi + BLE in powersave mode
- MU retries in DL MUMIMO are more in CoEx
- While executing WIFI Commissioning using the wifi\_station\_ble\_provisioning example, BLE is disconnecting is observed with few boards.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
### **System**
- None
## Limitations and Unsupported Features
### **System**
- None
### **Wi-Fi/Network Stack**
- TLS 1.3 Server is not supported
- Broadcast TWT is not supported.
- AMSDU TX is not supported.
- Fragmentation is not supported.
- AMSDUs within AMPDU is not supported.
- Currently, the module does not support Radio Measurement Requests feature of CCX V2
- 802.11k is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- Maximum of 3 SSL connections are supported in Wi-Fi alone mode.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- Legacy TA(NWP) LP mode is not supported.
- In iTWT mode, downlink data traffic should be limited. Disconnections may be observed if AP fails to send all the buffered data in the next TWT Service Period.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- UL MUMIMO is not supported.
- The embedded HTTP server is not supported.
- Baremetal is not supported
- Compared to SoC mode, NCP mode has fewer examples, but more will be enabled in upcoming releases.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- UART 2 Debug prints are supported only on ULP\_GPIO\_9.
- WiFi Mesh not supported
- Apple HomeKit not supported
- Cellular coexistence not supported
- The maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
### **BLE**
- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported(1 Central connection and 1 Peripheral connection) or (2 Central connections) or (2 Peripheral connections)).  
- BLE Slave latency value is valid up to 32 only.
- BLE TX/RX throughput is less when tested with EFM as compared to EFR.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported. 
- EATT feature is not supported.
- Periodic Advertising with response(PAwR) feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.
## **Simplicity Studio and Commander**
- All projects in the package are compatible with GCC 12.2.2 version.


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