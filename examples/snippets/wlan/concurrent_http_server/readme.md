# Wi-Fi - Concurrent HTTP Server

## High-Level Overview

SiWx91x concurrent HTTP server example: Run Wi-Fi station and soft AP together, serve a provisioning webpage on the AP instance, and connect the STA to a third-party access point using credentials entered on the webpage.

## Table of Contents

- [Wi-Fi - Concurrent HTTP Server](#wi-fi---concurrent-http-server)
  - [High-Level Overview](#high-level-overview)
  - [Table of Contents](#table-of-contents)
  - [Purpose/Scope](#purposescope)
  - [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
    - [Hardware Requirements](#hardware-requirements)
    - [Software Requirements](#software-requirements)
  - [Getting Started](#getting-started)
  - [Application Build Environment](#application-build-environment)
  - [Test the Application](#test-the-application)
  - [Steps to create webpage](#steps-to-create-webpage)
  - [Steps to add additional information on webpage](#steps-to-add-additional-information-on-webpage)
  - [Troubleshooting](#troubleshooting)
  - [Resources](#resources)
  - [Report Bugs and Get Support](#report-bugs-and-get-support)

## Purpose/Scope

This application demonstrates how to configure the HTTP Server in SiWx91x in concurrent mode, i.e., in both Wi-Fi Station mode (STA instance) and Access Point mode (SoftAP instance).
In this example application, the SiWx91x's AP instance acts as an HTTP Server and the client instance can connect to a wireless Access Point using the webpage that is served by HTTP Server.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- SoC Mode:
  - Standalone
    - BRD4002A Wireless Pro Kit Mainboard [SI-MB4002A](https://www.silabs.com/development-tools/wireless/wireless-pro-kit-mainboard?tab=overview)
    - Radio Boards 
	  - BRD4338A [SiWx917-RB4338A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-rb4338a-wifi-6-bluetooth-le-soc-radio-board?tab=overview)
	  - BRD4342A [SiWx917-RB4342A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx91x-rb4342a-wifi-6-bluetooth-le-soc-radio-board?tab=overview)
	  - BRD4339B [SiWx917-RB4339B](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-at)
	  - BRD4340A [SiWx917-RB4340A](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-at)
	  - BRD4343A [SiWx917-RB4343A](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4343a-wi-fi-6-bluetooth-le-8mb-flash-radio-board-for-module?tab=overview)
	  - BRD4343C [SiWx917-RB4343C](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4343c-wi-fi-6-bluetooth-le-8mb-flash-radio-board-for-module?tab=overview)
  - Kits
    - SiWx917 AC1 Module Explorer Kit [BRD2708A](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-ek2708a-explorer-kit?tab=overview)

- NCP Mode:
  - [BRD4346A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-rb4346a-wifi-6-bluetooth-le-soc-4mb-flash-radio-board?tab=overview) + [BRD8045C](https://www.silabs.com/development-tools/wireless/wi-fi/shield-adapter-board-for-co-processor-radio-boards?tab=overview)
  - [BRD4357A](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4357a-wi-fi-6-bluetooth-le-4mb-flash-radio-board-for-rcp-and-ncp-modules?tab=overview) + [BRD8045C](https://www.silabs.com/development-tools/wireless/wi-fi/shield-adapter-board-for-co-processor-radio-boards?tab=overview)
  - [BRD4357C](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4357c-wi-fi-6-bluetooth-le-4mb-flash-radio-board-for-rcp-and-ncp-modules?tab=overview) + [BRD8045C](https://www.silabs.com/development-tools/wireless/wi-fi/shield-adapter-board-for-co-processor-radio-boards?tab=overview)
  - Silicon Labs [BRD4180B](https://www.silabs.com/development-tools/wireless/slwrb4180b-efr32xg21-wireless-gecko-radio-board?tab=overview)
  - Host MCU Eval Kit. This example has been tested with:
    - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)


### Software Requirements

- Simplicity Studio


## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- [Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-simplicity-studio)
- [Install WiSeConnect extension](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-the-wi-se-connect-extension)
- [Connect your device to the computer](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#connect-si-wx91x-to-computer)
- [Upgrade your connectivity firmware ](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#update-si-wx91x-connectivity-firmware)
- [Create a Studio project ](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#create-a-project)

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

The application can be configured to suit your requirements and development environment. Read through the following sections and make any changes needed.

  - Configure the following parameters in ``app.c`` to test the HTTP Server app as per requirements:

  - HTTP Server Port and IP Settings

    ```c
    #define HTTP_SERVER_PORT           80                 // Http server port
    ```

- AP instance related parameters

	- CHANNEL_NUMBER refers to the specific frequency band the Access Point operates on, which should match the client mode's AP for successful communication.

  	```c
  	#define CHANNEL_NUMBER                     1
    ```

	- WIFI_AP_PROFILE_SSID refers to the SSID of the WiSeConnect softAP that would be created.

  	```c
  	#define WIFI_AP_PROFILE_SSID                   "MY_AP_SSID"
  	```

	- WIFI_AP_CREDENTIAL refers to the secret key of the WiSeConnect softAP that would be created.

  	```c
  	#define WIFI_AP_CREDENTIAL                     "MY_AP_PASSPHRASE"
    ```

NOTE:
> 1. In concurrent mode, STA and AP should be configured on the same channel. 
> 2. Valid values for CHANNEL_NO are 1 to 14 in the 2.4 GHz band. The default region code is set to US.
> 3. The `sli_si91x_accept_async` function in sl_http_server_start is not thread-safe.
> 4. For WPA3 security (including WPA3 Personal and WPA3 Personal Transition modes), enable the SL_SI91X_EXT_FEAT_IEEE_80211W bit in the .ext_custom_feature_bit_map field of boot configuration.

NOTE:
> Procedure to run the example:
> 1. Flash the application onto your device and run it. Upon successful execution, the SiWx91x will act as an Access Point (AP) with the SSID set to WIFI_AP_PROFILE_SSID and the password set to WIFI_AP_CREDENTIAL.
>
> 2. The application will initialize a server in AP mode and add default request handlers for the "/login" and "/connect" URIs.
>
> 3. Once the server is started, connect a client device to the server. Open a web browser on the client device and navigate to the IP address of the SiWx91x module with the URI set to /login. The default IP address is 192.168.10.10, so you can access it using 192.168.10.10/login.
>
> 4. On the webpage, enter the correct SSID, Password, and Security Type, then click on Connect.
>
> 5. The server will stop, and the credentials from the webpage will be retrieved through a "/connect" POST request.
>
> 6. The SiWx91x will then connect to the AP specified on the webpage in client mode.
>
> 7. After serving the request, the HTTP Server will be stopped and deinitialized.

> **Note**: For recommended settings, please refer the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application.
>
> Note: 
> - The default SSID is "MY_AP_SSID" and passphrase is "MY_AP_PASSPHRASE". You may either use these or modify them as described in the [Application Build Environment](#application-build-environment) section.
- Flash, run, and debug the application.

   ![Webpage](resources/readme/webpage.png)

   ![Application prints](resources/readme/output1.png)
   ![Application prints](resources/readme/output2.png)

  ## Steps to create webpage
  >Steps to update login handler:
  >1. Develop a new webpage similar to login.h.
  >2. Register the new request handler in  sl_http_server_handler_t structure in app.c.
  >3. Implement a new handler in app.c and update the data field in the server response structure accordingly.

  > Steps to update connect handler
  >1. Modify the open() method in the login.h file.
  >2. Modify the action attribute of the form tag in the login.h file.
  >3. Register the new request handler in  sl_http_server_handler_t structure in app.c.

  ## Steps to add additional information on webpage
  > If the user wants to add additional information to the webpage, they need to update the login.h file.

## Troubleshooting

If you encounter issues while running the Concurrent HTTP Server example, check the following:

- Verify the default SSID (`MY_AP_SSID`) and passphrase (`MY_AP_PASSPHRASE`) in `sl_net_default_values.h`, or update them as described in [Application Build Environment](#application-build-environment).
- Connect a client device to the SiWx91x soft AP and open the served webpage in a browser before you enter third-party AP credentials.
- Enter the correct SSID, password, and security type on the webpage. The server stops after the `/connect` POST request, and the STA connects using those credentials.
- If the STA fails to connect, confirm that the third-party AP is reachable and that the credentials match the target network.
- For custom webpages, verify that new handlers are registered in `sl_http_server_handler_t` in `app.c` as described in [Steps to create webpage](#steps-to-create-webpage).
- For NCP mode, verify that the host interface (SPI or UART) matches the project variant.

## Resources

- [WiSeConnect Getting Started Guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/)
- [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure)
- [WiSeConnect Recommended Settings Guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/)

## Report Bugs and Get Support

Report issues and get help from the Silicon Labs community:

- [Silicon Labs Community](https://www.silabs.com/community)
