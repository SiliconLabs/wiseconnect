# SL FILE SYSTEM

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Overview](#overview)
- [About Example Code](#about-example-code)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [Test the Application](#test-the-application)

## Purpose/Scope

- File system application demonstrates about updating a file named boot_count every time main runs

## Overview

- This example interfaces with flash through QSPI interface using littlefs.
- The program can be interrupted at any time without losing track of how many times it has been booted and without corrupting the filesystem.

## About Example Code

- \ref file_system_example.c this example code demonstates how to configure the qspi to access the flash for file system using littlefs library.
- Initialize the QSPI for littlefs using \ref sl_si91x_littlefs_qspi_init() 
- To mount the filesystem calls the  lfs_mount()
- Opens the file and reads current boot count 
- Updates the boot count
- Close and unmouns the the filesystem

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(4002A) + BRD4339B]

### Software Requirements

- Simplicity Studio
- Serial console Setup
  - The Serial Console setup instructions are provided below:
Refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-soc-mode#perform-console-output-and-input-for-brd4338-a).

### Setup Diagram

> ![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

## Test the Application

1. Run the application 
2. Observe boot_count on console output
 > ![Figure: Build run and Debug](resources/readme/file_system_output.PNG)

 >
> **Note**:
>
>- When we run the littlefs example first time, after flash erase we will see the error **(error: Corrupted dir pair at {0x0, 0x1})** because flash is not yet formatted for littlefs. So ignore the error when run first time.



