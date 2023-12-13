# PSA ChachaPoly for SI91X

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
  - [Application Configuration Parameters](#application-configuration-parameters)
- [Test the Application](#test-the-application)

## Purpose/Scope 

This application contains an example code to demonstrate the PSA ChachaPoly functionality.

## Prerequisites/Setup Requirements

Before running the application, the user will need the following things to setup.

### Hardware Requirements	

  - Windows PC 
  - Silicon Labs [Si917 Evaluation Kit WSTK + BRD4338A]

### Software Requirements

- Simplicity Studio

### Setup Diagram

  ![Figure: Introduction](resources/readme/image508a.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

## Application Build Environment

- To program the device ,refer **"Burn M4 Binary"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio.
 
### Application Configuration Parameters

 * Wrapped key support is provided for AES,Chachapoly,HMAC,GCM with CBC/ECB mode. This example demonstrate the use of wrapped key with AES-ChachaPoly using wrap CBC mode.
 * To use wrapped key, set USE_WRAPPED_KEYS macro to 1 in `psa_chachapoly_app.c`
 * To use software fallback instead of hardware accelerators for plain key:
  - Add mbedtls_chachapoly in component section of slcp file
  - Undefine the macro SLI_AEAD_DEVICE_SI91X

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application.
- Flash, run and debug the application.

Follow the steps as mentioned for the successful execution of the application:

[AN1311: Integrating Crypto Functionality Using PSA Crypto Compared to Mbed TLS Guide](https://www.silabs.com/documents/public/application-notes/an1311-mbedtls-psa-crypto-porting-guide.pdf)
