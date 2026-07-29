# Wi-Fi Provisioning via Access Point

## High-Level Overview

SiWx91x provisioning example: Configure the device as a provisioning access point and HTTP server to receive third-party AP credentials from a client browser in SoC and NCP modes.

This example illustrates Wi-Fi provisioning using Access Point (AP) mode. It demonstrates how to display available Wi-Fi networks on an HTTP server and allows the device to connect to a third-party Access Point (AP) via the STA instance. Users can view and select networks from the HTTP interface, enabling straightforward connection to the chosen network.

## Table of Contents

- [Wi-Fi Provisioning via Access Point](#wi-fi-provisioning-via-access-point)
	- [Table of Contents](#table-of-contents)
	- [High-Level Overview](#high-level-overview)
	- [Purpose/Scope](#purposescope)
	- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
		- [Hardware Requirements](#hardware-requirements)
		- [Software Requirements](#software-requirements)
		- [Setup Diagram](#setup-diagram)
	- [Getting Started](#getting-started)
	- [Application Build Environment](#application-build-environment)
	- [Run the Application](#run-the-application)
		- [Connecting to a Wi-Fi Network](#connecting-to-a-wi-fi-network)
	- [Troubleshooting](#troubleshooting)
	- [Resources](#resources)
	- [Report Bugs and Get Support](#report-bugs-and-get-support)

## Purpose/Scope

The app starts in Wi-Fi access point (AP) mode, creating a local network that users can connect to with their device. Once connected, users access a local web server hosted by the SiWx91x module through their browser. This web server facilitates the provisioning process by allowing users to scan for available Wi-Fi networks, select their desired network, and enter the network credentials.

After successfully provisioning, the app transitions from AP mode to Wi-Fi Station mode. In Station mode, the SiWx91x module connects to the selected Wi-Fi network. Users can then continue to interact with the device via their browser to monitor or control the device's functionalities.

All device activities can be observed on the serial terminal prints. You may use a readily available terminal program such as [Tera Term](https://teratermproject.github.io/index-en.html) or [PuTTY](https://www.putty.org/).

## Prerequisites/Setup Requirements

 Before running the application, you will need the following things for setup.

### Hardware Requirements

- Windows PC
- Wireless Access Point
- **SoC Mode**:
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
	- SiWG917 Dev Kit [BRD2605A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-dk2605a-wifi-6-bluetooth-le-soc-dev-kit?tab=overview)
    - SiWx917 AC1 Module Explorer Kit [BRD2708A](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-ek2708a-explorer-kit?tab=overview)

- NCP Mode:
  - [BRD4346A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-rb4346a-wifi-6-bluetooth-le-soc-4mb-flash-radio-board?tab=overview) + [BRD8045C](https://www.silabs.com/development-tools/wireless/wi-fi/shield-adapter-board-for-co-processor-radio-boards?tab=overview)
  - [BRD4357A](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4357a-wi-fi-6-bluetooth-le-4mb-flash-radio-board-for-rcp-and-ncp-modules?tab=overview) + [BRD8045C](https://www.silabs.com/development-tools/wireless/wi-fi/shield-adapter-board-for-co-processor-radio-boards?tab=overview)
  - [BRD4357C](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4357c-wi-fi-6-bluetooth-le-4mb-flash-radio-board-for-rcp-and-ncp-modules?tab=overview) + [BRD8045C](https://www.silabs.com/development-tools/wireless/wi-fi/shield-adapter-board-for-co-processor-radio-boards?tab=overview)
  - Silicon Labs [BRD4180B](https://www.silabs.com/development-tools/wireless/slwrb4180b-efr32xg21-wireless-gecko-radio-board?tab=overview)
  - Host MCU Eval Kit. This example has been tested with:
    - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)


### Software Requirements

- [Simplicity Studio](https://www.silabs.com/developers/simplicity-studio)


### Setup Diagram

![Figure: Setup Diagram for SiWx91x module](resources/readme/setup.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- [Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-simplicity-studio)
- [Install WiSeConnect extension](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-the-wi-se-connect-extension)
- [Connect your device to the computer](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#connect-si-wx91x-to-computer)
- [Upgrade your connectivity firmware ](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#update-si-wx91x-connectivity-firmware)
- [Create a Studio project ](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#create-a-project)

## Application Build Environment

The application can be configured to suit your requirements and development environment. Read through the following sections and make any changes needed.

1. In the Project explorer pane, expand the **config** folder and open the **sl_net_default_values.h** file.
- **SiWx91x Provisioning AP instance related parameters**

	- DEFAULT_WIFI_AP_PROFILE_SSID refers to the SSID of the SiWx91x Provisioning AP that would be created.

  	```c
  	#define DEFAULT_WIFI_AP_PROFILE_SSID                   "MY_AP_SSID"
  	```

	- DEFAULT_WIFI_AP_CREDENTIAL refers to the secret key of the SiWx91x Provisioning AP that would be created.

  	```c
  	#define DEFAULT_WIFI_AP_CREDENTIAL                     "MY_AP_PASSPHRASE"
  	```

	-  HTTP_SERVER_PORT refers to the port number of the SiWx91x Provisioning AP's HTTP server that would be created.

  	```c
  	#define HTTP_SERVER_PORT 80
  	```

> **Note**: For recommended settings, please refer the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

## Run the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application.
- Flash, run, and debug the application.

**Step 1**: The SiWx91x module starts in AP mode. All device activities can be observed on the serial terminal prints.

**Step 2**: Connect your laptop/mobile to the SiWx91x module's AP. The default network name and password are `MY_AP_SSID` and `MY_AP_PASSPHRASE`, respectively.

**Step 3**: Launch the browser 

### Connecting to a Wi-Fi Network

![Connecting to a Network Connections](resources/readme/connecting-to-a-network-connections.png)

**Step 1** : Make sure that your Laptop/mobile device is connected to the SiWx91x module's AP network.

**Step 2** : Enter `192.168.10.10` on your browser to open the provisioning webpage.

![Provisioning Webpage](resources/readme/provisioning-webpage.png)

**Step 3** : Click `Scan` to scan for available networks. 

![Scan](resources/readme/scan.png)

**Step 4** : Select your desired network, enter password, and then click `Connect`.

![Selecting a Network](resources/readme/selecting-a-network.png)

**Step 5** : The SiWx91x module switches to Wi-Fi Station mode and connects to the selected Wi-Fi network. 

![Connected Prints](resources/readme/connected-prints.png)

## Troubleshooting

If you encounter issues while running this example, check the following:

- Connect your client device to the SiWx91x provisioning AP and open the provisioning webpage in a browser.
- Enter the correct third-party AP SSID, passphrase, and security type on the provisioning page.
- If STA connection fails after provisioning, verify the credentials and confirm the target AP is in range.

## Resources

- [WiSeConnect Getting Started Guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/)
- [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure)
- [WiSeConnect Recommended Settings Guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/)

## Report Bugs and Get Support

Report issues and get help from the Silicon Labs community:

- [Silicon Labs Community](https://www.silabs.com/community)
