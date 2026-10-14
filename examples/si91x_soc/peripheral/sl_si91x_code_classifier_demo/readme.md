# SiWx91x Platform - PSRAM Code Classifier Demo

## Table of Contents

- [SiWx91x Platform - PSRAM Code Classifier Demo](#siwx91x-platform-psram-code-classifier-demo)
  - [Table of Contents](#table-of-contents)
  - [Purpose/Scope](#purposescope)
  - [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
    - [Hardware Requirements](#hardware-requirements)
    - [Software Requirements](#software-requirements)
    - [Setup Diagram](#setup-diagram)
  - [Getting Started](#getting-started)
  - [PSRAM Code Classifier Usage](#psram-code-classifier-usage)
    - [**Installing the PSRAM Code Classifier Component**](#installing-the-psram-code-classifier-component)
  - [Memory Placement](#memory-placement)
    - [**1. Text and Data to SRAM**](#1-text-and-data-to-sram)
    - [**2. BSS to PSRAM**](#2-bss-to-psram)
    - [**3. Customizing Memory Placement**](#3-customizing-memory-placement)
      - [**SL_SI91X_CODE_CLASSIFY Macro**](#sl_si91x_code_classify-macro)
  - [Testing the Application](#testing-the-application)
  - [Troubleshooting](#troubleshooting)
  - [Resources](#resources)
  - [Report Bugs/Support](#report-bugssupport)

## Purpose/Scope

This example project demonstrates the use of the **PSRAM Code Classifier** component specifically for **PSRAM memory management**. It showcases how to classify and allocate variables efficiently between **PSRAM and SRAM**, ensuring optimized performance for applications using external PSRAM.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs SiWx91x Evaluation Kit [WPK(BRD4002) + BRD4342A]

### Software Requirements

- Simplicity Studio

### Setup Diagram

 ![Figure: setupdiagram](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- [Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#install-simplicity-studio)
- [Install WiSeConnect extension](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#install-the-wiseconnect-3-extension)
- [Connect your device to the computer](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#connect-siwx91x-to-computer)
- [Upgrade your connectivity firmware](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#update-siwx91x-connectivity-firmware)
- [Create a Studio project](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#create-a-project)

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## PSRAM Code Classifier Usage

This application demonstrates the usage of the **PSRAM Code Classifier** feature for PSRAM-based memory management. It categorizes variables into specific memory classes, ensuring optimal performance when using external PSRAM.

The following classifier categories are available for use (for **reference** to help users categorize variables accordingly):

- `SL_CODE_CLASS_FORCE_PSRAM`: Used for variables placed in external **PSRAM** for bulk storage.
- `SL_CODE_CLASS_FORCE_RAM`: Ensures placement of frequently accessed variables in **internal RAM**.
- `SL_CODE_CLASS_DMA_ACCESSIBLE`: Used for variables that need to be **accessible by the DMA**.
- `SL_CODE_CLASS_TIME_CRITICAL`: Applied to **time-sensitive operations**, minimizing access latency.

### **Installing the PSRAM Code Classifier Component**
To use this feature, the **PSRAM Code Classifier Component** must be installed from the **Software Component Selection** in Simplicity Studio.

## Memory Placement

To optimize performance, memory placement is categorized as follows:

### **1. Text and Data to SRAM**
- **All text and initialized data is placed in PSRAM** by default.
- The **PSRAM Code Classifier** can be used to selectively move specific elements to RAM if required for performance optimization.
- Use the following **prefixes** for classification:
  - `text_` → Used for **code placement** in PSRAM.
  - `data_` → Used for **initialized data** in PSRAM.

### **2. BSS to PSRAM**
- The **default BSS section is placed in RAM** to ensure optimal execution.
- If needed, **specific BSS variables can be placed in PSRAM** using the **PSRAM Code Classifier**.
- Use the following **prefix** for classification:
  - `bss_` → Used for **uninitialized data** to allocate the same to PSRAM if required.

### **3. Customizing Memory Placement**

Variables and functions can be explicitly placed into the desired memory regions using the **PSRAM Code Classifier** macro `SL_SI91X_CODE_CLASSIFY`.

#### **SL_SI91X_CODE_CLASSIFY Macro**

The `SL_SI91X_CODE_CLASSIFY` macro is used to classify and place code, data, and variables into specific memory sections based on their usage requirements. This allows fine-grained control over memory placement for optimal performance.

**Macro Syntax:**
```c
SL_SI91X_CODE_CLASSIFY(<segment>, <component>, <classifier_type>)
```

**Parameters:**
- `<segment>`: Specifies the memory segment type. Common values include:
  - `text` - For code/functions (text segment)
  - `data` - For initialized data variables
  - `bss_to_psram` - For uninitialized data to be placed in PSRAM
  - `data_to_ram` - For initialized data to be placed in RAM
  - `data_dma_accessible` - For DMA-accessible data
  - `data_time_critical` - For time-critical data variables

- `<component>`: A unique identifier/name for the variable or function being classified. This is used internally to create unique section names.

- `<classifier_type>`: The memory class to apply. Available options:
  - `SL_CODE_CLASS_FORCE_PSRAM` - Force placement in external PSRAM
  - `SL_CODE_CLASS_FORCE_RAM` - Force placement in internal RAM
  - `SL_CODE_CLASS_DMA_ACCESSIBLE` - Ensure DMA accessibility
  - `SL_CODE_CLASS_TIME_CRITICAL` - Optimize for time-sensitive operations

**Possible Combinations:**

1. **Placing BSS variables in PSRAM:**
   ```c
   SL_SI91X_CODE_CLASSIFY(bss_to_psram, psram_variable, SL_CODE_CLASS_FORCE_PSRAM)
   volatile int psram_large_buffer[1024];
   ```

2. **Placing initialized data in RAM for fast access:**
   ```c
   SL_SI91X_CODE_CLASSIFY(data_to_ram, ram_variable, SL_CODE_CLASS_FORCE_RAM)
   volatile int fast_access_counter = 0;
   ```

3. **Making data DMA-accessible:**
   ```c
   SL_SI91X_CODE_CLASSIFY(data_dma_accessible, dma_variable, SL_CODE_CLASS_DMA_ACCESSIBLE)
   volatile int dma_transfer_buffer[2] = { 0, 1 };
   ```

4. **Classifying time-critical variables:**
   ```c
   SL_SI91X_CODE_CLASSIFY(data_time_critical, time_critical_variable, SL_CODE_CLASS_TIME_CRITICAL)
   volatile bool time_critical_event_triggered = false;
   ```

5. **Classifying functions for time-critical code:**
   ```c
   SL_SI91X_CODE_CLASSIFY(text, app_api, SL_CODE_CLASS_TIME_CRITICAL)
   void code_classifier_example_process_action(void)
   {
       // Function implementation
   }
   ```

**Important Notes:**
- The macro must be placed **immediately before** the variable or function declaration.
- The `<component>` parameter should be unique for each classification to avoid naming conflicts.
- The macro is only effective when the **PSRAM Code Classifier Component** is installed and enabled.
- For recommended settings, please refer to the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

## Application Build Environment

- Configure the following macros in [`code_classifier.c`](code_classifier.c) file and update/modify following macros, if required.

  - `LED_INSTANCE`: Selects the LED instance toggled by the example to visually indicate application execution. By default, it is set to `led_led0`.

    ```c
    #define LED_INSTANCE    led_led0 // LED instance
    ```

  - `TOGGLE_DELAY_MS`: Delay (in milliseconds) between successive LED toggles. By default, it is set to 1000.

    ```c
    #define TOGGLE_DELAY_MS 1000     // Toggle delay in ms
    ```

## Testing the Application

1. Compile and flash the application onto the WPK base board.
2. Observe LED0 blinking at the configured blink rate.

## Troubleshooting

- If the project does not build, ensure Simplicity Studio and the WiSeConnect extension are installed and the board is connected.
- If the device is not detected, reinstall the connectivity firmware and check USB drivers.

## Resources

- [WiSeConnect Getting Started](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/)
- [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/)
- [SiWx91x SoC Documentation](https://docs.silabs.com/wiseconnect/latest/)

## Report Bugs/Support

For issues and support, use the Silicon Labs Community or your normal support channel.

