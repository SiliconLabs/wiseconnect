# SL ANALOG COMPARATOR

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
  - [Macros for Clock Configurations](#macros-for-clock-configurations)
  - [Macros for SYSRTC Configurations](#macros-for-sysrtc-configurations)
- [Test the Application](#test-the-application)

## Purpose/Scope
- This example demonstrates analog comparator functionality. If non-inverting input voltage is greater than inverting input voltage then analog comparator output will be high and analog comparator interrupt will hit.
- Following Configuration are used in example :
  - Non-inverting input pin 
  - Inverting input pin    
  - Hystersis value
  - Filter enable/disable 
  - Comparator instance used for application
  - Threshold and scale factor values for internal voltages

## Overview
 - Analog comparators peripheral consists of two analog comparators, a reference buffer, a scaler and a resistor bank.

- The comparator compares analog inputs p and n to produce a digital output, cmp_out according to:
  - p > n, cmp_out = 1
  - p < n, cmp_out = 0
- Both comparators can take inputs from GPIOs.
- There are 9 different inputs for each pin of comparator, and 2 of the 9 are external pin inputs (GPIOs).
- The following cases of comparison are possible
Compare external pin inputs
Compare external pin input to internal voltages.
Compare internal voltages.
- The inputs of 2 comparators can be programmed independently. The reference buffer, scaler and resistor bank are shared between the two comparators and can be enabled only when atleast one of the comparators is enabled.

## About Example Code
- \ref analog_comparator_example.c this example file demonstrates how to use analog comparator to compare external pin inputs, external pin input to internal voltages and internal voltages.
- In this example first analog comparator is initialized by enabling clocks through \ref sl_si91x_analog_comparator_init API.
- Sets non-inverting, inverting inputs, hysterisis value and filter for the comparator used through \ref sl_si91x_analog_comparator_set_configurations API(inputs are configured as per the usecase macros enabled).
- Registers callback for comparator interrupts and enable its interrupts through \ref sl_si91x_analog_comparator_register_callback API 
### If 'COMPARE_POS_INPUT_EXTERNAL_NEG_INPUT_EXTERNAL' macro is enabled:
 - Then comparator will compare external voltages, applied externally.
### if 'COMPARE_POS_INPUT_EXTERNAL_NEG_INPUT_REF_SCALER' is enabled:
- Then comparator will compare external input with internal reference scaler voltage & sets reference scaler output by setting its scale factor value through \ref sl_si91x_analog_comparator_set_reference_scaler_output.
### if 'COMPARE_POS_INPUT_EXTERNAL_NEG_INPUT_RESISTOR_BANK' is enabled:
- Then comparator will compare external input with internal resistor bank voltage and sets threshold value for resistor bank output voltage through \ref sl_si91x_analog_comparator_set_resistor_bank_threshold.
### if 'COMPARE_POS_INPUT_RESISTOR_BANK_NEG_INPUT_REF_SCALER' is enabled:
- Then comparator will compare resistor bank output with internal reference scaler voltage and sets reference scaler output by setting its scale factor value \ref sl_si91x_analog_comparator_set_reference_scaler_output.
- Also sets threshold value for resistor bank output voltage through \ref sl_si91x_analog_comparator_set_resistor_bank_threshold.
### if 'COMPARE_POS_INPUT_OPAMP_NEG_INPUT_EXTERNAL' is enabled:
- Then comparator will compare OPAMP output with external voltage and initializes opamp through opamp_init() function.
### if 'COMPARE_POS_INPUT_OPAMP_NEG_INPUT_REF_SCALER' is enabled:
- Then comparator will compare OPAMP output with reference scalar output and initializes opamp through opamp_init() function.
- Also sets reference scaler output by setting its scale factor value through \ref sl_si91x_analog_comparator_set_reference_scaler_output.
### if 'COMPARE_POS_INPUT_OPAMP_NEG_INPUT_RESISTOR_BANK' is enabled:
- Then comparator will compare OPAMP output with resistor bank output and initializes opamp through opamp_init() function.
- Also sets threshold value for resistor bank output voltage through \ref sl_si91x_analog_comparator_set_resistor_bank_threshold.
### if 'COMPARE_POS_INPUT_DAC_NEG_INPUT_EXTERNAL' is enabled:
- Then comparator will compare DAC output with external voltage and initializes DAC through DAC_init() function & sets DAC input value & sampling rate.
### if 'COMPARE_POS_INPUT_DAC_NEG_INPUT_REF_SCALER' is enabled:
- Then comparator will compare DAC output with reference scalar output and initializes DAC through DAC_init() function & sets DAC input value & sampling rate.
- Also sets reference scaler output by setting its scale factor value through \ref sl_si91x_analog_comparator_set_reference_scaler_output.

- In all above usecases if non-inverting input voltage is greater than inverting input voltage then comparator interrupt will occur and it will toggle ULP-GPIO-5 (for comparator-1) or ULP-GPIO-1 (for comparator-2) continously unless non-inverting voltage is greater.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4338A / BRD4342A / BRD4343A ]

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

- Open **sl_si91x_analog_comparator.slcp** project file select **software component**tab and search for **analog comparator** in search bar.
- Click on **comparator2** and configure its parameters from wizard.
- For using any other timer instance user has to add that timer instance by clicking on **ULP Timer Instance** from configuration wizard and then clicking on **Add New Instance**
- For creating analog comparator-1 instances write 'comparator1', then click on **Done**
- After creation of instances separate configuration files are get generated in **config folder**.
- If project built without selecting configurations, it will take default values from UC.

  ![Figure: UC-Screen](resources/uc_screen/analog_comparator_uc_screen.png)
  
### Application Configuration Parameters

- Configure the following macros in analog_comparator_example.c file and update/modify following macros if required.
```C
// Update below macro as per instance used for application.
// Update '1' for using comparator1  & '2' for using comparator2
#define ANALOG_COMPARATOR_USED     2

// Enable below macro to compare external inputs. 
// For this comparision select 'External input-1' as non-inverting input & 
// inverting input from UC, so both inputs will fed from GPIOs.
#define COMPARE_POS_INPUT_EXTERNAL_NEG_INPUT_EXTERNAL  ENABLE 

// Enable below macro to compare external input to reference scaler output. 
// For this comparision select 'External input-1' as non-inverting input & 
// select 'Reference scaler' as inverting input from UC.
#define COMPARE_POS_INPUT_EXTERNAL_NEG_INPUT_REF_SCALER  DISABLE 

// Enable below macro to compare external input to resistor bank output.
// For this comparision select 'External input-1' as non-inverting input &
// select 'Resistor bank output' as inverting input from UC.
#define COMPARE_POS_INPUT_EXTERNAL_NEG_INPUT_RESISTOR_BANK  DISABLE 

// Enable below macro to compare reference scaler output to resistor bank output,
// For this comparision select 'Resistor bank output' as non-inverting input &
// select 'Reference scaler output' as inverting input from UC.
#define COMPARE_POS_INPUT_REF_SCALAR_NEG_INPUT_RESISTOR_BANK DISABLE

// Enable below macro to compare OPAMP output with external voltage
// For this comparision select 'OPAMP1 output' as non-inverting input &
// select 'External input-1' as inverting input from UC.
#define COMPARE_POS_INPUT_OPAMP_NEG_INPUT_EXTERNAL DISABLE

// Enable below macro to compare OPAMP output with reference scaler voltage
// For this comparision select 'OPAMP1 output' as non-inverting input &
// select 'Reference scaler output' as inverting input from UC.
#define COMPARE_POS_INPUT_OPAMP_NEG_INPUT_REF_SCALER DISABLE

// Enable below macro to compare OPAMP output internal resistor bank voltage
// For this comparision select 'OPAMP1 output' as non-inverting input &
// select 'Resistor bank output' as inverting input from UC.
#define COMPARE_POS_INPUT_OPAMP_NEG_INPUT_RESISTOR_BANK DISABLE

// Enable to compare DAC output with external voltage
// For this comparision select 'DAC output' as non-inverting input &
// select 'External input-1' as inverting input from UC.
#define COMPARE_POS_INPUT_DAC_NEG_INPUT_EXTERNAL DISABLE

// Enable to compare DAC output with external voltage
// For this comparision select 'DAC output' as non-inverting input &
// select 'Reference scaler output' as inverting input from UC.
#define COMPARE_POS_INPUT_DAC_NEG_INPUT_REF_SCALER DISABLE
```
- Note :
  1. At a time only one MACRO should be enable from above Compare MACROs.
  2. For A0 boards use Comparator-1
  3. For B0 boards use Comparator-2   
  4. 4.To compare external input to buffer output enable macro 'COMPARE_POS_INPUT_EXTERNAL_NEG_INPUT_EXTERNAL' and 
  select 'External input-1' as non-inverting input & 'Reference buffer ouput' as inverting input from UC. Reference buffer
  will produce a fixed output of 1.2v only.
```C
// Change below macro value to change resistor bank output voltage, 
// for possible values \ref sl_analog_comparator_threshold_values_t enum present in sl_si91x_analog_comparator.h file.
#define THRESHOLD_VALUE 
// Change below macro value to change reference scaler output voltage, 
// for possible values refer \ref sl_analog_comparator_scale_factor_values_t present in sl_si91x_analog_comparator.h file. 
#define SCALE_FACT_VAL 
```
  
### Pin Configuration

|GPIO pin (A0) |GPIO pin (B0 1.2)|GPIO pin (B0 2.0)| Description|
|--- | --- | --- | --- |
|ULP_GPIO_0 [EXP_HEADER-9] |GPIO_27 [EXP_HEADER-10]|GPIO_27 [P29] |Non-inverting input|
|ULP_GPIO_1(P16) |GPIO_28 [EXP_HEADER-8]|GPIO_28 [P31]          |Inverting input    |
|ULP_GPIO_5 [EXP_HEADER-13] |ULP_GPIO_1 [P16]|ULP_GPIO_1 [P16]|Output Pin         |

## Test Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

1. Compile and run the application. 

2. If 'COMPARE_POS_INPUT_EXTERNAL_NEG_INPUT_EXTERNAL' MACRO is enabled :
  - Connect external voltage to Non-inverting input and Inverting input.     
  
3. If 'COMPARE_POS_INPUT_EXTERNAL_NEG_INPUT_REF_SCALER' MACRO is enabled :
  - Connect external voltage to non-inverting input.
  - Inverting input will be internally connected to reference scaler output so leave it open.
  
4. If 'COMPARE_POS_INPUT_EXTERNAL_NEG_INPUT_RESISTOR_BANK' MACRO is enabled :
  - Connect external voltageNon-inverting input.
  - Inverting input will be internally connected to resister bank output so leave it open.     
  
5. If 'COMPARE_POS_INPUT_RESISTOR_BANK_NEG_INPUT_REF_SCALAR' MACRO is enabled :
  - Non-inverting input will be internally connected to the resistor bank output.
  - Inverting input will be internally connected to the reference scaler output.

6. If 'COMPARE_POS_INPUT_OPAMP_NEG_INPUT_REF_SCALER' MACRO is enabled :
  - Non-inverting input will be internally connected to opamp output, but connect external voltage to opamp1 input (ULP_GPIO_7) to get opamp output.
  - Inverting input will be internally connected to the reference scaler output.

7. If 'COMPARE_POS_INPUT_OPAMP_NEG_INPUT_RESISTOR_BANK' MACRO is enabled :
  - Non-inverting input will be internally connected to opamp output, but connect external voltage to opamp1 input (ULP_GPIO_7) to get opamp output.
  - Inverting input will be internally connected to the resister bank output.

8. If 'COMPARE_POS_INPUT_OPAMP_NEG_INPUT_EXTERNAL' MACRO is enabled :
  - Non-inverting input will be internally connected to opamp output, but connect external voltage to opamp1 input (ULP_GPIO_7) to get opamp output.
  - Connect external voltage to Inverting input.

9. If 'COMPARE_POS_INPUT_DAC_NEG_INPUT_EXTERNAL' MACRO is enabled :
  - Non-inverting input will be internally connected to DAC output. Change value of 'DAC_INPUT_SAMPLE_VALUE' macro to change DAC input, its value should be less than MAX_DAC_INPUT_SAMPLE_VALUE macro value.
  - Connect external voltage to Inverting input.

10. If 'COMPARE_POS_INPUT_DAC_NEG_INPUT_REF_SCALER' MACRO is enabled :
  - Non-inverting input will be internally connected to DAC output. Change value of 'DAC_INPUT_SAMPLE_VALUE' macro to change DAC input, its value should be less than MAX_DAC_INPUT_SAMPLE_VALUE macro value.
  - Inverting input will be internally connected to the reference scaler output.

## Expected Results 
### For Comparator-1:
  - If Applied Non-inverting input supply >= Applied Inverting input supply, then the comparator output will be high, interrupt will hit & ULP_GPIO_5 will toggles unless comparator output is high. Also below debug prints will be observed. Observe the ULP_GPIO_5 status by using a logic analyzer. 

    ![Figure: Comparator1_output](resources/readme/comparator1_output1.png)

  - If Applied Non-inverting input supply < Applied Inverting input supply, then comparator output will be low, interrupt will not hit, ULP_GPIO_5 will not toggle and below debugs will be there on console.Observe the ULP_GPIO_5 status by using a logic analyzer.

    ![Figure: Comparator1_output](resources/readme/comparator1_output2.png)

### For Comparator-2:
  - If Applied Non-inverting input supply >= Applied Inverting input supply, then the comparator output will be high, interrupt will hit & ULP_GPIO_1 will toggles unless comparator output is high. Also below debug prints will be observed. Observe the ULP_GPIO_1 status by using a logic analyzer.                

    ![Figure: Comparator2_output](resources/readme/comparator2_output1.png)

  - If Applied Non-inverting input supply < Applied Inverting input supply, then comparator output will be low, interrupt will not hit, ULP_GPIO_1 will not toggle and below debugs will be there on console.Observe the ULP_GPIO_1 status by using a logic analyzer.

    ![Figure: Comparator2_output](resources/readme/comparator2_output2.png)


> **Note:**
>
> - Interrupt handlers are implemented in the driver layer, and user callbacks are provided for custom code. If you want to write your own interrupt handler instead of using the default one, make the driver interrupt handler a weak handler. Then, copy the necessary code from the driver handler to your custom interrupt handler.
