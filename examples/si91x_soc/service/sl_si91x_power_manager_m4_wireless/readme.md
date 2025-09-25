# SL POWER MANAGER M4 WIRELESS

## Table of Contents

- [SL POWER MANAGER M4 WIRELESS](#sl-power-manager-m4-wireless)
  - [Table of Contents](#table-of-contents)
  - [Purpose/Scope](#purposescope)
  - [Overview](#overview)
  - [About Example Code](#about-example-code)
  - [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
    - [Hardware Requirements](#hardware-requirements)
    - [Software Requirements](#software-requirements)
    - [Setup Diagram](#setup-diagram)
  - [Getting Started](#getting-started)
  - [Test the Application](#test-the-application)

## Purpose/Scope

- This application demonstrates the power manager service APIs, state transitions, sleep-wakeup (with RAM retention) and sleep-wakeup (without RAM retention).

## Overview

- According to the macros configured in power_manager_example.h file, it executes the state transition and sleep-wakeup with RAM retention on button press, sleep-wakeup without RAM retention using second trigger as wakeup source.
- By default, the system is configured to the PS3 state. The application switches to the PS4 state as required. The state transitions occurs when the button is pressed. State transition is demonstrated in the following manner: PS4 -> PS2, PS2 -> PS4, PS4 -> PS4 Sleep -> PS4, PS4 -> PS3, PS3 -> PS3 Sleep -> PS3, PS3 -> PS2, PS2 -> PS2 Sleep -> PS2, PS2 -> PS1 -> PS2, PS2 -> PS3, PS3 -> PS4. It switches the states when the button is pressed.
- Sleep - wakeup without RAM retention is demonstrated as follows: PS4 -> PS0 -> wakeup -> restart the controller.
- For the integration of power manager in other projects, refer the instructions at the given path: **examples/si91x_soc/service/power_manager_m4_wireless/resources/power_manager_integration_guide/power_manager_integration.pdf** 

## About Example Code

- At initialization, a thread is created and the application_start() function is called along the thread.
- All the activities are handled in the application_start() function.
- Firstly wifi is initialized, M4-NWP secure handshake is established to send commands to NWP, NWP is switched to STANDBY_WITH_RAM_RETENTION mode.
- Power Manager service is initialized, the processor is switched to PS3 state and the clock is at 40 MHz (Power Save) using  sl_si91x_power_manager_init. The application will switch to the PS4 state.
- All the possible events are ored and passed to the  sl_si91x_power_manager_subscribe_ps_transition_event along with the callback function address.

- Upon button press, it changes the state and performs sleep-wakeup operations.
  - PS4 -> PS2: Unwanted peripherals are powered off using  sl_si91x_power_manager_remove_peripheral_requirement, PS2 state requirement is added using  sl_si91x_power_manager_add_ps_requirement.
  - PS2 -> PS4: To transmit to PS2, remove the requirement for the PS2 state using  sl_si91x_power_manager_remove_ps_requirement and add a requirement for the PS4 state using  sl_si91x_power_manager_add_ps_requirement switches the power state to PS4.
  - PS4 -> PS4 Sleep -> PS4:
    - Wakeup Source is selected as the calendar second trigger. The calendar peripheral is initialized before setting it as a wakeup source, the calendar is initialized using  sl_si91x_calendar_init, the second trigger is selected as wakeup source using  sl_si91x_power_manager_set_wakeup_sources, Now callback is registered for second trigger (it enables the trigger also) using  sl_si91x_calendar_register_sec_trigger_callback.
    - Now soc goes to sleep using  sl_si91x_power_manager_sleep. Upon wake-up, the calendar is stopped using  sl_si91x_calendar_rtc_stop and callback is unregistered using  sl_si91x_calendar_unregister_sec_trigger_callback.

  - PS4 -> PS3: To transmit to PS3, remove the requirement for PS4 state using  sl_si91x_power_manager_remove_ps_requirement and add the requirement for PS3 state using  sl_si91x_power_manager_add_ps_requirement switches the power state to PS3.
  - PS3 -> PS3 Sleep -> PS3:
    - Wakeup Source is selected as the calendar second trigger. The calendar peripheral is initialized before setting it as a wakeup source, the calendar is initialized using  sl_si91x_calendar_init, the second trigger is selected as wakeup source using  sl_si91x_power_manager_set_wakeup_sources, Now callback is registered for second trigger (it enables the trigger also) using  sl_si91x_calendar_register_sec_trigger_callback.
    - Now soc goes to sleep using  sl_si91x_power_manager_sleep. Upon wake-up, the calendar is stopped using  sl_si91x_calendar_rtc_stop and callback is unregistered using  sl_si91x_calendar_unregister_sec_trigger_callback.

  - PS3 -> PS2: Unwanted peripherals are powered off using  sl_si91x_power_manager_remove_peripheral_requirement. To transmit to PS2, remove the requirement for PS3 state using  sl_si91x_power_manager_remove_ps_requirement and add the requirement for PS3 state using  sl_si91x_power_manager_add_ps_requirement switches the power state to PS3.
  - PS3 -> PS2: Unwanted peripherals are powered off using  sl_si91x_power_manager_remove_peripheral_requirement. To transmit to PS2, remove the requirement for PS3 state using  sl_si91x_power_manager_remove_ps_requirement and add the requirement for PS3 state using  sl_si91x_power_manager_add_ps_requirement switches the power state to PS3.
  - PS2 -> PS2 Sleep -> PS2:
    - Wakeup Source is selected as the calendar second trigger. The calendar peripheral is initialized before setting it as a wakeup source, the calendar is initialized using  sl_si91x_calendar_init, the second trigger is selected as wakeup source using  sl_si91x_power_manager_set_wakeup_sources, Now callback is registered for second trigger (it enables the trigger also) using  sl_si91x_calendar_register_sec_trigger_callback.
    - Now soc goes to sleep using  sl_si91x_power_manager_sleep.
    - Upon wakeup, the calendar is stopped using  sl_si91x_calendar_rtc_stop and the callback is unregistered using  sl_si91x_calendar_unregister_sec_trigger_callback.
  - PS2 -> PS1 -> PS2:
    - Wakeup Source is selected as ULP Timer. ULP Timer peripheral is initialized before setting it as a wakeup source. It is initialized using [sl_si91x_ulp_timer_init](https://docs.silabs.com/wiseconnect/3.5.0/wiseconnect-api-reference-guide-si91x-peripherals/ulp-timer#sl-si91x-ulp-timer-init). Set the match value for 2 seconds and configure the ULP Timer using [sl_si91x_ulp_timer_set_configuration](https://docs.silabs.com/wiseconnect/3.5.0/wiseconnect-api-reference-guide-si91x-peripherals/ulp-timer#sl-si91x-ulp-timer-set-configuration). Now callback is registered for the timeout event (it enables the trigger also) using [sl_si91x_ulp_timer_register_timeout_callback](https://docs.silabs.com/wiseconnect/3.5.0/wiseconnect-api-reference-guide-si91x-peripherals/ulp-timer#sl-si91x-ulp-timer-register-timeout-callback). ULPSS-based wakeup source is selected using sl_si91x_power_manager_set_wakeup_sources.
    - Now the timer is started using [sl_si91x_ulp_timer_start](https://docs.silabs.com/wiseconnect/3.5.0/wiseconnect-api-reference-guide-si91x-peripherals/ulp-timer#sl-si91x-ulp-timer-start). The requirement is added for PS1 state using sl_si91x_power_manager_add_ps_requirement.
    - Upon wakeup, the timer is stopped using [sl_si91x_ulp_timer_stop](https://docs.silabs.com/wiseconnect/3.5.0/wiseconnect-api-reference-guide-si91x-peripherals/ulp-timer#sl-si91x-ulp-timer-stop), callback is unregistered using [sl_si91x_ulp_timer_unregister_timeout_callback](https://docs.silabs.com/wiseconnect/3.5.0/wiseconnect-api-reference-guide-si91x-peripherals/ulp-timer#sl-si91x-ulp-timer-unregister-timeout-callback) and the wakeup source is cleared using sl_si91x_power_manager_set_wakeupsources.  
  - PS2 -> PS3: To transmit to PS3, remove the requirement for the PS2 state using  sl_si91x_power_manager_remove_ps_requirement and add the requirement for the PS3 state using  sl_si91x_power_manager_add_ps_requirement switches the power state to PS3.
  - PS3 -> PS4: To transmit to PS3, remove the requirement for PS3 state using  sl_si91x_power_manager_remove_ps_requirement and add the requirement for PS4 state using  sl_si91x_power_manager_add_ps_requirement switches the power state to PS4.
  - PS4 -> PS0 -> Restarts the soc
    - The NWP is switched to DEEP_SLEEP_WITHOUT_RAM_RETENTION which means, sleep without retention.
    - Wakeup Source is selected as the calendar second trigger. The calendar peripheral is initialized before setting it as a wakeup source, the calendar is initialized using  sl_si91x_calendar_init, the second trigger is selected as wakeup source using  sl_si91x_power_manager_set_wakeup_sources, Now callback is registered for second trigger (it enables the trigger also) using  sl_si91x_calendar_register_sec_trigger_callback.
    - It goes to PS0 state using  sl_si91x_power_manager_add_ps_requirement. After waking up using the calendar one-second trigger, it restarts the controller.  

## Prerequisites/Setup Requirements

- To use this application following Hardware, Software and Project Setup is required.

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4338A / BRD4342A / BRD4343A ]
- SiWx917 AC1 Module Explorer Kit (BRD2708A)

### Software Requirements

- Simplicity Studio
- Serial console Setup
  - For Serial Console setup instructions, refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#console-input-and-output).
- Embedded Development Environment
  - For Silicon Labs Si91x, use the latest version of Simplicity Studio (refer **"Download and Install Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html**)

### Setup Diagram

![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) for the following tasks:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

> **Note**: For recommended settings, see the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

## Test the Application

Refer instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) for the following tasks:

1. Compile and run the application.
2. Enable macros whose functionality needs to be tested.
3. By default it initializes the wifi and switches NWP to standby with RAM retention mode, power manager service is initialized and callback is subscribed. Press the button to change the power state.
4. After successful program execution, the prints in the serial console look as shown below.

   ![Figure: Introduction](resources/readme/output.png)
