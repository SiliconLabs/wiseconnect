# Code Classifier

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Code Classifier Usage](#code-classifier-usage)
- [Memory Placement](#memory-placement)
- [Testing the Application](#testing-the-application)

## Purpose/Scope

This example project demonstrates the use of the **Code Classifier** component specifically for **PSRAM memory management**. It showcases how to classify and allocate variables efficiently between **PSRAM and RAM**, ensuring optimized performance for applications using external PSRAM.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4342A]

### Software Requirements

- Simplicity Studio

### Setup Diagram

 ![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- [Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-simplicity-studio)
- [Install WiSeConnect 3 extension](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-the-wi-se-connect-3-extension)
- [Connect your device to the computer](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#connect-si-wx91x-to-computer)
- [Upgrade your connectivity firmware](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#update-si-wx91x-connectivity-firmware)
- [Create a Studio project](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#create-a-project)
- Compile and run the application.

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Code Classifier Usage

This application demonstrates the usage of the **Code Classifier** feature for PSRAM-based memory management. It categorizes variables into specific memory classes, ensuring optimal performance when using external PSRAM.

The following classifier categories are available for use (for **reference** to help users categorize variables accordingly):

- `SL_CODE_CLASS_FORCE_PSRAM`: Used for variables placed in external **PSRAM** for bulk storage.
- `SL_CODE_CLASS_FORCE_RAM`: Ensures placement of frequently accessed variables in **internal RAM**.
- `SL_CODE_CLASS_DMA_ACCESSIBLE`: Used for variables that need to be **accessible by the DMA**.
- `SL_CODE_CLASS_TIME_CRITICAL`: Applied to **time-sensitive operations**, minimizing access latency.

### **Installing the Code Classifier Component**
To use this feature, the **Code Classifier Component** must be installed from the **Software Component Selection** in Simplicity Studio. 

## Memory Placement

To optimize performance, memory placement is categorized as follows:

### **1. Text and Data to SRAM**
- **All text and initialized data is placed in PSRAM** by default.
- The **Code Classifier** can be used to selectively move specific elements to RAM if required for performance optimization.
- Use the following **prefixes** for classification:
  - `text_` → Used for **code placement** in PSRAM.
  - `data_` → Used for **initialized data** in PSRAM.

### **2. BSS to PSRAM**
- The **default BSS section is placed in RAM** to ensure optimal execution.
- If needed, **specific BSS variables can be placed in PSRAM** using the **Code Classifier**.
- Use the following **prefix** for classification:
  - `bss_` → Used for **uninitialized data** to allocate the same to PSRAM if required.

### **3. Customizing Memory Placement**
- Variables and functions can be explicitly placed into the desired memory regions using the **Code Classifier**.
- The classifier macro format is:

  ```c
  SL_SI91X_CODE_CLASSIFY(<region>, <variable_name>, <classifier_type>)

## Testing the Application

1. Compile and flash the application onto the WPK base board.
2. Observe LED0 blinking at the configured blink rate.

