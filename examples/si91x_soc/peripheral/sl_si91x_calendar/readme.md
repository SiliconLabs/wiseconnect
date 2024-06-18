# SL CALENDAR

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
  - [Application Configuration Parameters](#application-configuration-parameters)
- [Test the Application](#test-the-application)

## Purpose/Scope

- This example demonstrates the clock configuration, one millisecond trigger, one second trigger, alarm configuration and time conversion.

  > **Note:** Before enabling any trigger, it is recommended to calibrate the clock.

## Overview

-	Calendar calculates milliseconds, seconds, minutes, hours, days, months and years up to 4 centuries. 
-	It also calculates days of week and takes care of number of days in month as well as leap year. 
-	It can also configure alarm for desired time as a one-shot trigger. 
-	It can generate triggers on one second and one millisecond time interval. 
-	It uses APB for read and write operations in real time. 
-	RC clock and RO clock are configurable, and it can also be calibrated using the APIs. 

## About Example Code

- This example demonstrates clock configuration, setting calendar date-time, retrieving calendar date-time, setting alarm date-time, retrieving alarm date-time, handling alarm triggers, one-millisecond triggers, one-second triggers, and clock calibration. 
- To configure the calendar clock, select the clock from UC. The \ref sl_si91x_calendar_config function is used to set the calendar clock. 
- A structure is created containing default values for calendar date-time. It is created using \ref sl_si91x_calendar_build_datetime_struct. After entering all the parameters, it returns a structure filled with all the parameters. 
- Calendar date-time is configured using the \ref sl_si91x_calendar_set_date_time API. It configures the date-time, and the calendar blocks start counting from that time. 
- To verify if the desired time is set, the \ref sl_si91x_calendar_get_date_time API is used. It returns a structure with the current date-time. 

- If **ALARM_EXAMPLE** macro is enabled:

  - A date-time structure is created using \ref sl_si91x_calendar_build_datetime_struct for configuring the alarm.  
  - Alarm is configured using \ref sl_si91x_calendar_set_alarm API.
  - Now callback is registered to perform action at the time of trigger using \ref sl_si91x_calendar_register_alarm_trigger_callback API.
  - To verify if the desired alarm is set, \ref sl_si91x_calendar_get_alarm API is used, It returns a structure which has configured alarm date-time.
  - At the time of trigger, it prints current date-time on the console.

- If **CLOCK_CALIBRATION** macro is enabled:
  
  - This clock calibration applies to RO and RC Clock only. For RO clock \ref sl_si91x_calendar_roclk_calibration should be used and for RC clock \ref sl_si91x_calendar_rcclk_calibration should be used.
  - It is recommended to calibrate clock before activating any trigger after every power cycle.
  - Initialization of clock is performed using \ref sl_si91x_calendar_calibration_init API.
  - To select the clock in UC, follow the procedure mentioned in "Configuration and Steps for Execution" section.
  - According to the clock configured in UC, either RO or RC, use respective API to configure the clock. In this example RC clock is selected so \ref sl_si91x_calendar_rcclk_calibration API is used.
  - This API expects \ref clock_calibration_config_t structure. For rc_trigger_time, \ref RC_CLOCK_CALIBRATION_ENUM enum can be used, and for ro_trigger_time \ref RO_CLOCK_CALIBRATION_ENUM enum can be used.
  - After calibration \ref sl_si91x_calendar_rtc_start is called to start the calendar clock.

- If **SEC_INTR** macro is enabled:

  - Callback is registered for one second trigger using \ref sl_si91x_calendar_register_sec_trigger_callback API.
  - At every one second, the console prints `one_sec_callback`.

- If **MILLI_SEC_INTR** macro is enabled:

  - Callback is registered for one millisecond trigger using \ref sl_si91x_calendar_register_msec_trigger_callback API.
  - At every one second, the console prints `on_msec_callback triggered 1000 times`.
  - It accumulates the 1000 trigger at one millisecond time frame and prints at only one second.

- If **TIME_CONVERSION** macro is enabled:

  - This converts NTP time to Unix Time and vice versa.
  - In this example, \ref sl_si91x_calendar_convert_unix_time_to_ntp_time API is called to convert unix time to ntp time, it expects unix time as parameter.
  - It updates the variable with ntp time which is passed as parameter.
  - After conversion, ntp time and unix time are printed on the console.
  - Now \ref sl_si91x_calendar_convert_ntp_time_to_unix_time API is used to convert ntp time to unix time, it expects ntp time as parameter.
  - It updates the variable with unix time which is passed as parameter.
  - After conversion, ntp time and unix time are printed on the console.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002)+ BRD4338A]
  - The Serial Console setup instructions are provided below:
Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-soc-mode#perform-console-output-and-input-for-brd4338-a).

### Software Requirements

- Simplicity Studio
- Serial console Setup
  - The Serial Console setup instructions are provided below:
Refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#console-input-and-output)

### Setup Diagram

> ![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

### Application Configuration Parameters

- Configure UC from the slcp component.
- Open **sl_si91x_calendar.slcp** project file select **software component** tab and search for **Calendar** in search bar.
- Using configuration wizard one can configure the Calendar clock, i.e., RO, RC and XTAL.

  ![Figure: Introduction](resources/uc_screen/calendar_uc_screen.png)

- Configuration file is generated in **config folder**, if not changed then the code will run on default UC values.
- Configure the following macros in calendar_example.h file and update/modify following macros if required.

- `ALARM_EXAMPLE`: If ALARM_EXAMPLE is enabled, it prints "Alarm Callback is Triggered" on console when alarm is triggered. By default, it is set to 0.

  ```C
    #define ALARM_EXAMPLE     0 // To enable alarm trigger 
  ```

- `CLOCK_CALIBRATION`: If CLOCK_CALIBRATION is enabled, after calibration "Successfully performed clock calibration" print is there on serial console. By default, it is set to 0.

  ```C
    #define CLOCK_CALIBRATION 0 // To enable clock calibration 
  ```

- `SEC_INTR`: If SEC_INTR is enabled, every one second "One Sec Callback is Triggered" print is there on serial console. By default, it is set to 0.

  ```C
    #define SEC_INTR          0 // To enable one second trigger 
  ```

- `MILLI_SEC_INTR`: If MILLI_SEC_INTR is enabled, every one second "One Milli-Sec Callback triggered 1000 times" print is there on serial console. By default, it is set to 0.

  ```C
    #define MILLI_SEC_INTR    0 // To enable one millisecond trigger 
  ```

- `TIME_CONVERSION`: If TIME_CONVERSION is enabled, time conversion between unix and ntp is printed on serial console. By default, it is set to 0.

  ```C
    #define TIME_CONVERSION   0 // To enable time conversion 
  ```

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

1. Compile and run the application.
2. By default, time and date is configured and prints are observed on serial console.
3. After successful program execution, the prints in serial console looks as shown below.

    ![Figure: Introduction](resources/readme/output.png)

> **Note:**
>
> - Interrupt handlers are implemented in the driver layer, and user callbacks are provided for custom code. If you want to write your own interrupt handler instead of using the default one, make the driver interrupt handler a weak handler. Then, copy the necessary code from the driver handler to your custom interrupt handler.
