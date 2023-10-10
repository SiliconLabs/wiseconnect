# Crypto HMAC

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

This application demonstrates how to configure and use the HMAC API and generates the digest for a given input.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- SoC Mode:
  - Silicon Labs [BRD4325A, BRD4325B, BRD4325C, BRD4325G, BRD4338A](https://www.silabs.com/)

### Software Requirements

- Simplicity Studio IDE

### Setup Diagram

#### SoC Mode 

![Figure: Setup Diagram SoC Mode for Crypto HMAC Example](resources/readme/setup_diagram_soc.png)

## Getting Started

Refer the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

## Application Build Environment

The application can be configured to suit your requirements and development environment. Read through the following sections and make any changes needed.

Open `app.c` file and configure the following parameters accordingly

- "msg" refers to plain data which is given to HMAC engine for encryption with all sizes of key.
- "key" refers to key which is used to encrypt/decrypt the "msg" using  HMAC engine.
- After filling the appropriate **sl_si91x_hmac_config_t** configuration, `sl_si91x_hmac()` stores the digest in the provided buffer. 

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application.
- Flash, run and debug the application.
- Based on the input data and key, the digest from HMAC engine can be observed in the provided buffer.

## Application Output

![HMAC Output](resources/readme/output.png)
