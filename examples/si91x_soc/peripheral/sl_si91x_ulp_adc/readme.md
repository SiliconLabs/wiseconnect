
# ULP ADC

## Introduction 
- This application demonstrate the ADC peripheral converts analog input to 12 bit digital output.
- The AUXADC can take analog inputs in single ended or differential ended. The output is 12 bit digital which can be given out with or without noise averaging.
- The Aux VRef can be connected directly to Vbat (Aux LDO bypass mode) or to the Aux LDO output.
- ADC will work on two operation mode i.e. FIFO mode and STATIC mode.
- The ADC channel configure will take care of set the pin mux as analog mode and gpio.
- The example ADC sampling the data and converts its to equivalent input voltage based on operation mode.

## Overview

- The ADC Controller works on a ADC with a resolution of 12bits at 10Msps when ADC reference Voltage is greater than 2.8v or 5Msps when ADC reference Voltage is less than 2.8v.
- Sample application will be 12 bit ADC Output in 2's complement representation.
- There are two operating mode in AUXADC controller:
  - Static Mode Operation
  - FIFO Mode Operation
- There a dedicated ADC DMA to support 16 channel.
- DMA mode supports dual buffer cyclic mode to avoid loss of data when buffer is full. In dual buffer cyclic mode, if buffer 1 is full for particular channel, incoming sampled 
  data is written into buffer 2 such that, samples from buffer 1 are read back by controller during this time. That’s why there are two start addresses, two buffer lengths and 
  two valid signals for each channel.
- External event-based sampling support.
- Multi-channel mode, as the multiple channels are sensed, after sensing each channel, information related to “buffer length” etc. of that particular channel are rewritten 
  in to the corresponding location of internal memory for the next use when same channel is sensed again.
- External triggers can be used in PS4 State to collect the sample from a pre-defined channel.

## Setting Up 
 - To use this application following Hardware, Software and the Project Setup is required

### Hardware Requirements 	
  - Windows PC 
  - SiWx917 Radio Board
 
![Figure: Introduction](resources/readme/image503a.png)

### Software Requirements
  - Si91x SDK
  - Embedded Development Environment
    - For Silicon Labs Si91x, use the latest version of Simplicity Studio (refer **"Download and Install Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html**)
 
## Project Setup
- **Silicon Labs Si91x** refer **"Download SDKs"**, **"Add SDK to Simplicity Studio"**, **"Connect SiWx917"**, **"Open Example Project in Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio

## Configuration and Steps for Execution

- Configure UC from the slcp component.
- Open **sl_si91x_adc.slcp** project file select **software component** tab and search for **ADC** in search bar.
- Using configuration wizard one can configure different parameters like:
  - **ADC Configuration**
  - Input Type: ADC input type can be configured, i.e. single ended and differential ended.
  - Operation mode: Can be configured operation mode of ADC, i.e. Static mode and FIFO mode.
  - Sampling rate: we can set sampling rate of ADC, i.e. bits/second it should be minimum is 1Kbps and maximum is 2.5Mbps.
  - Sample length: set the length of ADC samples, i.e. number of ADC sample collected for operation, it should be minimum is 1 and maximum is 1023.
  - Number of channel enable: Number of channels enabled for ADC data acquisition, i.e. for single channel we set as 1 and for multi channel we can set from 1 to 16.
  - POS/NEG input channel selection: In this we can set positive and negative input channel for ADC.

## Pin Configuration
| PIN   |A0 ULP GPIO PIN |B0 1.2 ULP GPIO PIN | B0 2.0 ULP GPIO PIN |Description |
| ---   | --- |--- | --- | --- |
| ADCN2 | ULP_GPIO_5 [EXP_HEADER-13] | ULP_GPIO_7  [EXP_HEADER-7] | ULP_GPIO_7  [EXP_HEADER-15]  |Connect to GND in Sigle ended mode |
| ADCP2 | ULP_GPIO_4 [EXP_HEADER-11] | ULP_GPIO_10 [EXP_HEADER-3] | ULP_GPIO_10 [P17]  |Connect to Analog input            |


## Build 
1. Compile the application in Simplicity Studio using build icon 

![Figure: Build run and Debug](resources/readme/image503c.png)

## Device Programming
- To program the device ,refer **"Burn M4 Binary"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio

## Executing the Application
In single ended mode give the input to ULP_GPIO_10 and in differential ended mode give positive 
input to ULP_GPIO_10 and negative input to ULP_GPIO_7 for B0 board, if any other refer pin configuration.
1. When the application runs, the ADC configure the settings as per the user and start ADC conversion.
2. After completion of conversion ADC input, it will print all the captured samples data in console.
3. Enable ULP_MODE macro in adc.example.c (#line38) for ulp mode of ADC.

## Expected Results 
After running the application it will store the equivalent input voltage from ADC output
samples in 'adc_input_eqv_vltg' buffer.
  - ADC output will print configured number of samples output voltage on UART console
  - Apply the different voltages(0V to Vref) to ADC input and observe console outputs as per input.
  - Provided input voltage and console output data should match.

## Formula Reference:
 - Use following formula to find equivalent input voltage of ADC
   **Differential Ended Mode:**
   vout = ((((float)ADC output/(float)4096) * Vref Voltage) - (Verf Voltage/2));
   **Exp**
   If Positive input to ADC given as 2.4V and Negative input given as 1.5 voltage then 
   ADC output will be digital value which is equivalent to 0.9V
   
   **Single ended Mode:** 
   vout = (((float)ADC output/(float)4096) * Vref Voltage);
   **Exp**
   If Positive input to ADC given as 2.4V then ADC output will be digital value which is equivalent to 2.4V
   
## Note
 - When enabling ULP_MODE macro this application will be executed from RAM.
 - In this application while changing the MCU mode from PS4 to PS2, M4 flash will be turned off.
 - The debug feature of Simplicity Studio will not work after M4 flash is turned off.
 - To check **Prints** for ADC example, connect the USB to TTL uart connector's RX_pin, 
    to the **EXP_HEADER-5** of the WPK[BRD4002A] Base Board.
	
## Expected Scenario:
 - After Flashing ULP examples as M4 flash will be turned off,flash erase does not work.
 - To Erase the chip follow the below procedure
   - Turn ON ISP switch and press the reset button → Turn OFF ISP Switch → Now perform Chip erase 
      through commander.
   - For B0 2.0(BRD4338A) borad:
   - Continous press the ISP button on radio board and press the reset button, then release the ISP button.	
