# Code Classifier

## Table of Contents

- [Code Classifier](#code-classifier)
  - [Table of Contents](#table-of-contents)
  - [Purpose/Scope](#purposescope)
  - [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
    - [Hardware Requirements](#hardware-requirements)
    - [Software Requirements](#software-requirements)
    - [Setup Diagram](#setup-diagram)
  - [Getting Started](#getting-started)
  - [Code Classifier Usage](#code-classifier-usage)
    - [**Installing the Code Classifier Component**](#installing-the-code-classifier-component)
  - [Memory Placement](#memory-placement)
    - [**1. Text and Data to SRAM**](#1-text-and-data-to-sram)
    - [**2. BSS to PSRAM**](#2-bss-to-psram)
    - [**3. Customizing Memory Placement**](#3-customizing-memory-placement)
      - [**SL_SI91X_CODE_CLASSIFY Macro**](#sl_si91x_code_classify-macro)
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

 ![Figure: setupdiagram](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- [Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#install-simplicity-studio)
- [Install WiSeConnect extension](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#install-the-wiseconnect-3-extension)
- [Connect your device to the computer](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#connect-siwx91x-to-computer)
- [Upgrade your connectivity firmware](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#update-siwx91x-connectivity-firmware)
- [Create a Studio project](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#create-a-project)

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Code Classifier Usage

This application demonstrates the **Code Classifier** for PSRAM-based memory management. When **TEXT segment in PSRAM** and **DATA segment in PSRAM** are installed, they define the default layout (typically text and initialized data in PSRAM). The classifier adds **selective** overrides (such as,  placing specific functions or data in internal RAM, placing the BSS segment in PSRAM using **`SL_CODE_CLASS_FORCE_PSRAM`**, or assigning DMA/time-critical sections as needed).

The following classifier categories are available for use (for **reference** to help users categorize variables accordingly). The macros **`SL_CODE_CLASS_FORCE_RAM`**, **`SL_CODE_CLASS_DMA_ACCESSIBLE`**, and **`SL_CODE_CLASS_TIME_CRITICAL`** all currently target **RAM** in link scripts. They differ by intent (performance, DMA, or latency), which keeps code self-documenting and allows future linker script updates (such as splitting alignment or memory regions) without requiring changes to the macro API.

- `SL_CODE_CLASS_FORCE_PSRAM`: Variables in external **PSRAM** for bulk storage.
- `SL_CODE_CLASS_FORCE_RAM`: Frequently accessed variables in **internal RAM** (performance intent).
- `SL_CODE_CLASS_DMA_ACCESSIBLE`: **DMA-accessible** placement (DMA-safe intent; currently the same RAM class).
- `SL_CODE_CLASS_TIME_CRITICAL`: **Time-sensitive** access (latency intent; currently the same RAM class).

### Installing the Code Classifier Component

To use this feature, the **Code Classifier Component** must be installed from the **Software Component Selection** in Simplicity Studio. 

## Memory Placement

To optimize performance, memory placement is categorized as follows. In source code, the macro’s first argument is the segment name (`text`, `data`, and so on). This argument expands to linker section names in the form `<segment>_<component>_<class>` (for example, `text_app_api_timecritical`), which the linker groups by prefix (`text_*`, `data_*`, and so on). Use the text segment for code and data for initialized data. The linker then maps the resulting sections according to your project’s linker scripts.

### 1. Text and Data to SRAM

- **All text and initialized data is placed in PSRAM** by default (when the corresponding PSRAM segment components are used).
- The **Code Classifier** can be used to selectively move specific elements to RAM if required for performance optimization.
- Section name prefixes from the macro relate to the PSRAM segment components as follows:
  - `text_*` → Used for **code** when the **TEXT segment in PSRAM** software component is installed.
  - `data_*` → Used for **initialized data** when the **DATA segment in PSRAM** software component is installed.

### 2. BSS to PSRAM

- The **default BSS section is placed in RAM** to ensure optimal execution.
- If needed, **specific BSS variables can be placed in PSRAM** using the **Code Classifier**.
- Use the following **prefix** for classification:
  - `bss_` → Used for **uninitialized data** to allocate the same to PSRAM if required.

### 3. Customizing Memory Placement

Variables and functions can be explicitly placed into the desired memory regions using the **Code Classifier** macro `SL_SI91X_CODE_CLASSIFY`.

#### **SL_SI91X_CODE_CLASSIFY Macro**

The `SL_SI91X_CODE_CLASSIFY` macro classifies code, data, and variables into memory sections. The first parameter is the **segment** name; the compiler emits a **section** `<segment>_<component>_<class>` for the linker to place (aligned with the Memory Placement summary above).

**Macro Syntax:**
```c
SL_SI91X_CODE_CLASSIFY(<segment>, <component>, <classifier_type>)
```

**Parameters:**
- `<segment>`: Memory segment name (first macro argument). Common values include:
  - `text` - Code/functions; linker collects `text_*` sections (for code when **TEXT segment in PSRAM** is part of the project).
  - `data` - Initialized data; linker collects `data_*` sections (for initialized data when **DATA segment in PSRAM** is part of the project).
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
- The macro is only effective when the **Code Classifier Component** is installed and enabled.
- For project-wide recommended settings, build options, and component combinations (including Code Classifier with PSRAM), see the [Recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

## Testing the Application

1. Compile and flash the application onto the WPK base board.
2. Observe LED0 blinking at the configured blink rate.
