# SL WATCHDOG TIMER

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
- [Expected Results](#expected-results)

## Purpose/Scope

- This example demonstrates the triggering of WDT warnings and LED toggling in the WDT interrupt handler continuously for 6 times. The system restarts (kicks) the WDT on every interrupt. On the sixth interrupt, the system does not restart the WDT, and then the WDT resets the application.
- Then, the WDT is started again with new parameters and the LED is toggled again for 6 times. Finally, the timer is stopped and de-initialized. 

## Overview

- The Watch Dog timer is used to generate interrupts on timeout and initiate a system reset in case of system failure, which can be caused by an external event like an ESD pulse or due to a software failure. Its interrupt can also be utilized as a wake-up source for the processor from SLEEP/STANDBY to ACTIVE states. 
- The Watchdog Timer generates interrupts and can be restarted (kicked) by the system upon every interrupt. There are two modes defined for the Watchdog Timer:  
  - Open Mode, during which Watchdog timer restarts are allowed from the CPU 
  - Closed Mode, during which Watchdog timer restarts are not allowed from the CPU. 
- The Processor needs to restart the Timer upon a timeout interrupt if the timer is not intended to reach the system-reset threshold. The timer will be in Closed Mode, as defined above, until the interrupt timer is reached. Once the interrupt timer is reached, it will be in Open Mode until the reset is generated. Also, upon interrupt generation, the timer restarts for the Reset Duration. 
- It has an independent window watchdog timer. 
- Interrupts are generated before the system reset is applied, which can be used as a wakeup source. 
- It generates a system reset upon Lockup indication from the Processor. 
- Configurable low and high-frequency FSM clock. 
- Configurable interrupt (timeout), system reset, and window period. 
- Able to operate when the CPU is in SLEEP state. 
- Individually controllable power domain for low-power applications. 

## About Example Code

-	\ref watchdog_timer_example.c This example file demonstrates how to use the Watchdog timer (WDT) to trigger WDT warnings and reset the system after a few warnings. With a WDT timeout interrupt occurring every 1 second, the WDT is restarted (kicked) by the application, and the onboard LED0 toggles. After toggling the LED 6 times, the application does not restart the WDT, then the timer loads the system-reset time (set to 4 seconds). Once that time is over, the WDT resets the system. Afterward, the WDT is started again with new parameters, and LED0 is toggled 6 times. Finally, the WDT is stopped, the callback is unregistered, and the timer is de-initialized. 
-	In this example, the application first toggles LED0 once and checks whether it's a power-on reset or a WDT system reset through the \ref sl_si91x_watchdog_get_timer_system_reset_status API.  
-	If it's a power-on reset, then the WDT is initialized by enabling peripheral power, enabling WDT to run during CPU sleep mode, and unmasking its interrupt through the \ref sl_si91x_watchdog_init_timer API.  
-	Then, the clock and timer are configured with default configuration values from UC through the \ref sl_si91x_watchdog_configure_clock and \ref sl_si91x_watchdog_set_configuration APIs, respectively.  
-	Next, the timer timeout callback is registered, and its interrupt is enabled using the \ref sl_si91x_watchdog_register_timeout_callback API.  
-	The WDT is then started using the \ref sl_si91x_watchdog_start_timer API. The application toggles onboard LED0 and restarts (kicks) the WDT on every interrupt (every 1 second) through the \ref sl_si91x_watchdog_restart_timer.  
-	Upon the 6th WDT interrupt, the application does not restart the WDT. So when the timer count reaches the system-reset time (4 seconds), it resets the application.  
-	After that, the application starts again, toggles LED0 once, checks WDT system reset status, and upon finding it true, debugs out "Watchdog-timer system-reset occurred".  
-	Then, the timer is initialized again, the callback is registered, and it's started with new parameters, configured using the following APIs:  
  - \ref sl_si91x_watchdog_set_system_reset_time to change the WDT system-reset time to 8 seconds. For possible values, refer to the \ref time_delays_t enum. 
 	- \ref sl_si91x_watchdog_set_interrupt_time to change the WDT interrupt time to 2 seconds. 
 	- \ref sl_si91x_watchdog_set_window_time to change the WDT window time to 32 milliseconds. 
-	To read the above time values, the following APIs are used:  
 	- \ref sl_si91x_watchdog_get_system_reset_time to read the system-reset time. 
 	- \ref sl_si91x_watchdog_get_interrupt_time to read the interrupt time. 
 	- \ref sl_si91x_watchdog_get_window_time to read the window time. 
-	Then, the application toggles onboard LED0 6 times and restarts (kicks) the WDT on every interrupt (every 2 seconds) through the \ref sl_si91x_watchdog_restart_timer. 
-	Upon the 6th WDT interrupt, the application does not restart the WDT and immediately stops the WDT through the \ref sl_si91x_watchdog_stop_timer API.  
-	Then, the callback is unregistered, and the timer is de-initialized through the \ref sl_si91x_watchdog_deinit_timer API. 


## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4338A]

### Software Requirements

- Simplicity Studio
- Serial console Setup
  - The Serial Console setup instructions are provided below:
Refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#console-input-and-output)

### Setup Diagram

![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

- Open **sl_si91x_watchdog_timer.slcp** project file select **software component**tab and search for **WDT** in search bar.
- Click on **SL_WDT** and configure the WDT as per configuration parameters given in wizard.
- If project built without selecting configurations, it will take default values from UC.

  ![Figure: UC](resources/uc_screen/watchdog_uc_screen.png)

- Configure Clock and timer using following macros, defined in \ref sl_si91x_watchdog_timer_config.h file and update/modify following macros if required:

### Macros for Clock Configurations

-	\ref SL_LOW_FREQ_FSM_CLK_SRC is used for configuring the low-frequency FSM clock. Refer to \ref low_freq_fsm_clock_t for more information. 
-	\ref SL_ULP_TIMER_CLK_ISL_BG_PMU_CLOCK_SRC is used for configuring the BG-PMU clock. Refer to \ref bg_pmu_clock_t for more information. 
-	\ref SL_ULP_TIMER_DIRECTION is set to true to enable waiting for switching timer clk, and false to skip waiting for switching timer clk. 
-	After configuring the above macros, their values are passed to the \ref watchdog_timer_clock_config_t structure type variable \ref sl_watchdog_timer_clk_config_handle, which is used to configure the clock through the API \ref sl_si91x_watchdog_configure_clock.


### Macros for Timer Configurations

-	\ref SL_WDT_SYSTEM_RESET_TIME is used for setting the system reset time. For possible values, refer to \ref time_delays_t. 
-	\ref SL_WDT_INTERRUPT_TIME (timeout time) is used for setting the timeout time. For possible values, refer to \ref time_delays_t. 
-	\ref SL_WDT_WINDOW_TIME is used for setting the window time. For possible values, refer to \ref time_delays_t. 
-	After configuring the above macros, their values are passed to the \ref watchdog_timer_config_t structure type variable \ref sl_watchdog_timer_config_handle, which is used to configure the timer through the API \ref sl_si91x_watchdog_set_configuration. 


## Test the Application

1. Compile and run the application.
2. The watchdog LED0 will be toggled, every 1 secs with WDT warning.
3. At 6th toggle, timer will reset the application after 4 seconds.
4. Then WDT starts with new parameters and toggles the LED0 again every 2 seconds.
5. After 6 toggles, stop toggling LED as the timer is stopped and de-initialized. Toggling of the LED can be seen in the image below. 

    ![Figure: Onboard LED0](resources/readme/image514d.png)

## Expected Results

- After successful program execution the prints in serial console looks as shown below.

  ![Figure: Introduction](resources/readme/output.png)

> **Note:**
>
> - Interrupt handlers are implemented in the driver layer, and user callbacks are provided for custom code. If you want to write your own interrupt handler instead of using the default one, make the driver interrupt handler a weak handler. Then, copy the necessary code from the driver handler to your custom interrupt handler.
