# PSRAM Driver Example

## Introduction 

- This example project demonstrates use of the PSRAM Driver APIs via QSPI interface. The example writes and reads data to and from PSRAM using auto mode, manual blocking mode and DMA mode. The application makes use of default configurations of PSRAM device with fast quad read and write in QPI (Quad IO) interface mode.

## Running Example Code

- To use this application following Hardware, Software and the Project Setup is required

### Hardware Requirements

- Windows PC
- Silicon Labs [Si917 Evaluation Kit WSTK/WPK + PSRAM supported Si917 radio board]

### Software Requirements

- Si91x SDK
- Embedded Development Environment
  - For Silicon Labs Si91x, use the latest version of Simplicity Studio (refer **"Download and Install Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html**)

## Project Setup

- **Silicon Labs Si91x** refer **"Download SDK"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio.
  
## Configuration and Steps for Execution

- PSRAM Read-Write type, interface mode, and operation frequency can be configured from "PSRAM Core" component under "Software Components" tab via GUI.
  
## Build

- Compile the application in Simplicity Studio using build icon.

## Device Programming

- To program the device ,refer **"Burn M4 Binary"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio.

## Executing the Application
- Compile and run the application.

## Expected Results 
 - Logs are printed with success or failure status for PSRAM read & write events in auto mode, manual mode, and manual via DMA mode

