# PSRAM Blinky

## Introduction  

- This example project demonstrates blinky application (GPIO/LED toggle) executing from PSRAM. By default, the Text segment, Data segment, BSS segment, Stack and Heap are placed in PSRAM.

## Running Example Code

- To use this application following Hardware, Software and the Project Setup is required

### Hardware Requirements

- Windows PC
- Silicon Labs [Si917 Evaluation Kit WSTK/WPK + BRD4340A Si917 radio board]

### Software Requirements

- Si91x SDK
- Embedded Development Environment
  - For Silicon Labs Si91x, use the latest version of Simplicity Studio (refer **"Download and Install Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html**)

## Project Setup

- **Silicon Labs Si91x** refer **"Download SDK"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio.

## Configuration and Steps for Execution

- Configure the following parameter in main.c file, update/modify following macro if required
```C
#define RSI_BLINK_RATE //  configured for 10 ticks per second   
```   
- Sections can be included in PSRAM by installing components present under "PSRAM Linker Configurations" from "SOFTWARE COMPONENTS" GUI. Same can be removed from PSRAM and placed into default memory by uninstalling the respective component from software component selection GUI. 

## Build

- Compile the application in Simplicity Studio using build icon.

## Device Programming

- To program the device ,refer **"Burn M4 Binary"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio.

## Executing the Application
- Compile and run the application.
- Toggles the state of LED0 at configured blink rate and then device goes to sleep 
**Note!** LED toggling and device sleep will be a continous process 

## Expected Results 
- LED0 should blink on WSTK base board.