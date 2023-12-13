# PSA Sha for SI91X

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [Test the Application](#test-the-application)

## Purpose/Scope

- This application contains an example code to demonstrate PSA SHA.

## Prerequisites/Setup Requirements 

Before running the application, the user will need the following things to setup.

### Hardware Requirements	

  - Windows PC 
  - Silicon Labs [Si917 Evaluation Kit WSTK + BRD4325A]

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

- The user can run the application to verify the following SHA modes:
- SHA 1
- SHA 224
- SHA 256
- SHA 384
- SHA 512
- Enable the desired SHA algorithm in psa_sha_app.h by enabling corresponding macro
- By defauit SHA 256 is enabled
* To use software fallback instead of hardware accelerators:
  - Add mbedtls_shaxxx in component section of slcp file
  - Undefine the macro SLI_SHA_DEVICE_SI91X

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application.
- Flash, run and debug the application.

Follow the steps as mentioned for the successful execution of the application:

* [AN1311: Integrating Crypto Functionality Using PSA Crypto Compared to Mbed TLS Guide](https://www.silabs.com/documents/public/application-notes/an1311-mbedtls-psa-crypto-porting-guide.pdf)

* [AN1135: Using Third Generation Non-Volatile Memory (NVM3) Data Storage](https://www.silabs.com/documents/public/application-notes/an1135-using-third-generation-nonvolatile-memory.pdf)