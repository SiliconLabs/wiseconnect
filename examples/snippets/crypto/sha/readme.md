# SiWx91x Platform Crypto SHA

## Table of Contents

- [SiWx91x Platform Crypto SHA](#platform-siwx91x-crypto-sha)
  - [Table of Contents](#table-of-contents)
  - [Purpose/Scope](#purposescope)
  - [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
    - [Hardware Requirements](#hardware-requirements)
    - [Software Requirements](#software-requirements)
    - [Setup Diagram](#setup-diagram)
      - [SoC Mode](#soc-mode)
  - [Getting Started](#getting-started)
  - [Application Build Environment](#application-build-environment)
  - [Test the Application](#test-the-application)
  - [Expected Results](#expected-results)
  - [Troubleshooting](#troubleshooting)
  - [Resources](#resources)
  - [Report Bugs/Support](#report-bugssupport)

## Purpose/Scope

In cryptography, SHA (Secure Hash Algorithm) is a cryptographic hash function designed by the United States National Security Agency and is a U.S. Federal Information Processing Standard published by the United States NIST.

SHA produces a message digest based on principles similar to those used by Ronald L. Rivest of MIT in the design of the MD4 and MD5 message digest algorithms, but has a more conservative design.

SHA forms part of several widely used security applications and protocols, including TLS and SSL, PGP, SSH, S/MIME, and IPsec. Those applications can also use MD5; both MD5 and SHA are descended from MD4.

- This application explains how to configure and use the SHA crypto engine using Silabs SDK.
- This Application demonstrates the various SHA operations like
  - SHA 1
  - SHA 256
  - SHA 384
  - SHA 512
- This application is used to compute digest using SHA crypto engine.
- This application is provided with single NIST test vector to verify funtionality.

## Prerequisites/Setup Requirements

### Hardware Requirements

  - Windows PC 
  - Silicon Labs [SiWx91x Evaluation Kit WSTK]

### Software Requirements

  - Embedded Development Environment
    - For Silicon Labs SiWx91x, use Simplicity Studio

### Setup Diagram

#### SoC Mode 

  ![Figure: Introduction](resources/readme/image502a.png)

## Getting Started

Refer the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

- Configure the following parameters in app.c file and update/modify following macros if required
- In calling function sl_si91x_sha, change SL_SI91X_SHA_512 to required sha mode
- From given configuration,
“SHA” refers to data which is given as input to SHA for computing digest.

> **Note**: For recommended settings, please refer the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

> **Note**: For recommended settings, please refer the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

> **Note**: To enable **sideband crypto**, Use **one** of the following methods, depending on your workflow:
>
> **Option 1 — Edit the example's `.slcp` file (the `.slcp` inside the example folder, not the project `.slcp`):**
>
> Add the `define` entry at example scope:
>
> ```yaml
> define:
>   - name: SL_SI91X_SIDE_BAND_CRYPTO
> ```
>
> **Option 2 — Add the macro using Configurators 2.0:**
>
> 1. Create the desired PSA example project and open it in any compatible IDE.
> 2. In the project explorer, right-click the project and select **Open Configurators 2.0**.
>
>    ![Right-click menu — Open Configurators 2.0](resources/readme/sideband_open_configurators.png)
>
> 3. In the configurator view, switch to the **Build Configurator** tab.
>
>    ![Configurators 2.0 — Build Configurator tab](resources/readme/sideband_build_configurator.png)
>
> 4. Under **Compiler Flags → C**, add `-DSL_SI91X_SIDE_BAND_CRYPTO` to the options list and save the configuration.
>
>    ![Build Configurator — Compiler Flags](resources/readme/sideband_compiler_flags.png)
>
> 5. **Clean** the project and then **rebuild** it for the change to take effect.

> [!NOTE]
> Sideband is supported only with PSA APIs.

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application.
- Flash, run and debug the application.

1. Depending on SHA mode digest of respective size is computed for the given input data.

## Expected Results

 - User will get the digest value.

![Output](resources/readme/output.png)

## Troubleshooting

For troubleshooting information, see [WiSeConnect Troubleshooting](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-troubleshooting/).

## Resources

- [WiSeConnect Documentation](https://docs.silabs.com/wiseconnect/latest/)
- [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/)

## Report Bugs/Support

To report bugs or get support, visit [Silicon Labs Community](https://community.silabs.com/).
