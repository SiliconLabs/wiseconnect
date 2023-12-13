# SENSOR HUB

## Table of Contents

- [Purpose/Scope](#purposescope)
- [About Example Code](#about-example-code)
- [Framework](#framework)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
  - [Sensor Hub Configuration Parameters](#sensor-hub-configuration-parameters)
- [Test the Application](#test-the-application)
- [Expected Results](#expected-results)

## Purpose/Scope

- This application demonstrates SensorHub service. 
- This service provides a convenient framework to integrate any sensor/s on I2C, GPIO, UART and ADC interfaces. It supports Sensor activity in low-power mode.

## About Example Code

- This application demonstrates the SensorHub service using the following sensors,
  * 3- I2C-based sensors namely a BH1750(Light Sensor), LM75(Temperature Sensor) and APDS9960(RGB Proximity Gesture sensor)
  * 2- ADC-based Sensors(Joystick and GUVA_12D_UV)
  * 1- GPIO-based Push Button-0 as sensor

- The application starts in PS4 mode, initializes peripherals, and configures sensors on the corresponding peripheral interfaces. Sensor data will be sampled and shown on the serial console based on the sensor configurations.

- The SensorHub service supports the following features:
  - Data Sample Modes:
      * Interrupt Mode
      * Polling Mode
  
  - Data Delivery Modes:
      * Polling Mode
          * Timeout
          * Threshold
          * Number of Samples 
      * Interrupt Mode
          * Interrupt (with GPIO-based and ADC-based)
  - Power State Transitions: 
      * PS4 Active > PS4 Sleep 
      * PS4 Active > PS2 Active
      * PS2 Active > PS2 Sleep
      * PS2 Active > PS1 
      * PS2 Active > PS4 Active
    
> **Note:** These transitions are not enabled by default. To enable, please use below mentioned configurations in 'PowerSave Configurations:' section.

## Framework


![Figure: Architecture](resources/readme/image508b.png)

- SensorHub has 4 tasks: 
  * Application task: This task handles the Create Sensor, Start Sensor, Stop Sensor, and Delete Sensor.
  * Event manager task: This task deals with processing the events of Sensor Hub and notifying the Application.
  * Sensor task: This task primarily involves managing sensors, including controlling them, obtaining data from the sensors, and dispatching events to the Event Manager.
  * Power manager task: this task handles switching the power states.

## Prerequisites/Setup Requirements

 - To use this application following Hardware, Software and Project Setup are required.

### Hardware Requirements	

- Silicon Labs Si917 Pro Kit [WPK + BRD4338A]
- BH1750 Light Sensor
- LM75 Temperature Sensor
- APDS9960 RGB-Gesture-Proximity Sensor
- 2 ADC-based Sensor(Joystick and GUVA_12D_UV)

### Software Requirements
- Si91x Wiseconnect
- Simplicity Studio
- Serial console-setup
  - The Serial console setup instructions are provided below, 
refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-soc-mode#perform-console-output-and-input-for-brd4338-a).

### Setup Diagram

![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) for the following tasks:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

## Application Build Environment

### SensorHub Configuration Parameters

1. Configure the number of sensor info in the ***/sensors/inc/sensors_config.h** file.
    ```c

    #define SL_MAX_NUM_SENSORS 5 // Maximum sensors present in the system
    ```

2. Configure the following parameters in the ***sensorhub_config.c*** file:
    * **Data Sample Modes:**
      - **Polling Sensor Mode** configures the below parameters:

        ```c

        .sensor_mode         = SL_SH_POLLING_MODE,
        .sampling_interval   = 100, 
        ```

        - If sensor_mode is selected as ***SL_SH_POLLING_MODE***, then data_deliver.data_mode should be configured as **one** of the following for a sensor configuration structure:
            - For **TIMEOUT Data Mode** configure the below parameters:

              ```c

              .data_deliver.data_mode          = SL_SH_TIMEOUT
              .data_deliver.timeout       = 1000
              ```

            - For **THRESHOLD Data Mode** configure the below parameters:

              ```c

              .data_deliver.data_mode          = SL_SH_THRESHOLD,  
              .data_deliver.threshold     = 1000,
              ```

            - For **SAMPLING Data Mode** configure the below parameters:

              ```c

              .data_deliver.data_mode          = SL_SH_NUM_OF_SAMPLES,  
              .data_deliver.numOfSamples  = 5,
              ```

            - **Example of Sensor Polling Mode Configurations**:-

              ```c

              .sensor_name               = "TEMPERATURE_SENSOR_LM75",
              .sensor_bus                = SL_SH_I2C,
              .sensor_id                 = SL_SENSOR_LM75_ID,
              .address                   = SL_LM75_ID,
              .sensor_mode               = SL_SH_POLLING_MODE,
              .sampling_interval         = 100,
              .data_deliver.data_mode    = SL_SH_NUM_OF_SAMPLES,
              .data_deliver.numofsamples = 5,
              ```

      - **INTERRUPT Sensor Mode** configures the below parameters:

        ```c

        .sensor_mode                = SL_SH_INTERRUPT_MODE,
        .sampling_intr_req_pin      = BUTTON_0_GPIO_PIN, 
        .sensor_intr_type           = SL_SH_FALL_EDGE,
        .data_deliver.data_mode     = SL_SH_NO_DATA_MODE,
        ```
          - **Example of Sensor Interrupt Mode Configurations:-**

            ```c

            //For I2C Sensor
            .sensor_name               = "LIGHT_SENSOR",
            .sensor_bus                = SL_SH_I2C,
            .sensor_id                 = SL_SENSOR_BH1750_ID,
            .address                   = SL_BH1750_ID,
            .sensor_mode               = SL_SH_INTERRUPT_MODE,
            .sampling_intr_req_pin     = BUTTON_0_GPIO_PIN,
            .sensor_intr_type          = SL_SH_FALL_EDGE,
            .data_deliver.data_mode    = SL_SH_NO_DATA_MODE,
            ```

            ```c
            //For ADC Sensor
            .sensor_name               = "ADC_JOYSTICK",
            .sensor_id                 = SL_SENSOR_ADC_JOYSTICK_ID,
            .channel                   = SL_SH_ADC_CH0_CHANNEL,
            .sensor_bus                = SL_SH_ADC,
            .sensor_mode               = SL_SH_INTERRUPT_MODE,
            .data_deliver.data_mode     = SL_SH_NO_DATA_MODE,
            ```

3. **PowerSave Configurations:**
    * The SensorHub utilizes the **Tickles Idle low-power mode** provided by the RTOS.
    * To configure the PS2, please update the below macro in the preprocessor settings:
      ```c
      SL_SENSORHUB_PS2_STATE=1
      //Enabling this macro will move the application from PS4 state to PS2 state. 
      //In PS2 state the sensor data will be sampled and collected.
      ```
    * To configure the power states to PS4 sleep or PS2 Sleep, please update the defines in ***\gecko_sdk_4.3.2\util\third_party\freertos\kernel\include\FreeRTOS.h** file as below:

      ```c
      #ifndef configUSE_TICKLESS_IDLE
      #define configUSE_TICKLESS_IDLE 1 // 1 is to Enable the tickless Idle mode 
      #endif

      #ifndef configPRE_SLEEP_PROCESSING
      #define configPRE_SLEEP_PROCESSING(x) sli_si91x_sleep_wakeup(x) // Here x is idle time, 
      #endif

      // Configure the sleep time by using the below macro. 
      // If the number of Ideal task ticks exceeds this value, the system is allowed to sleep.
      #ifndef configEXPECTED_IDLE_TIME_BEFORE_SLEEP
      #define configEXPECTED_IDLE_TIME_BEFORE_SLEEP    70
      #endif
      ```
    
    * To configure the power save transitions from PS2 to PS4 and vice-versa, please update the below macro in the  preprocessor settings:
      ```c
      SL_SH_POWER_STATE_TRANSITIONS = 1
      ```

4. **ADC Configurations**:
    - Configure the following parameters in the ***sensorhub_config.c*** file to change the ADC's mode from FIFO to STATIC and vice versa.

      ```c
      .adc_config.adc_cfg.operation_mode = SL_ADC_STATIC_MODE
      ```
    
    - The Joystick emulator and the GUVA sensor are compatible with both ADC static and FIFO modes. 
    
    - **ADC static mode:-**
        - SensorHub polling mode is supported
        - ADC is capable of reading only one Sample at a time and generates an interrupt in static mode.
        configure as shown below in the ***adc_sensor_hal.h*** file:

          ```C
            
          #define SL_SH_ADC_CH0_NUM_SAMPLES 1
          #define SL_SH_ADC_SENSOR0_NUM_OF_SAMPLES 5 
          ```  

    - **ADC Power Save(PS-1)**
      - The SensorHUB interrupt mode configurations are utilized in conjunction with **ADC FIFO mode**.
      - Configure the following parameter ***sensorhub_config.c*** file:

        ```c
        .adc_config.adc_cfg.operation_mode = SL_ADC_FIFO_MODE,
        ```

      - In the SensorHub interrupt mode, The ADC will continuously sample 4 channels, regardless of the number of channels enabled in sets of 4 (0-3).
      configure as shown below in the ***adc_sensor_hal.h*** file:

        ```C
          // Set SL_SH_ADC_CH0_NUM_SAMPLES to a value between 100 and 1023 if you are using sleep mode. 
          //Setting it to a value less than 100 may result in undefined behavior due to an immediate ADC interrupt.
          #define SL_SH_ADC_CH0_NUM_SAMPLES 100 

          #define SL_SH_ADC_SENSOR0_NUM_OF_SAMPLES 1
        ```

      - ADC can read between 1 and 1023 samples at a time and generates interrupts when operating in FIFO mode.
      - To configure the PS1 power state from the PS2 State, please update the below macro in the preprocessor settings.
          * Disable the tickles mode in the FreeRTOS.h file.
          * The PS1 state transition only applies to ADC FIFO Mode. Before entering this mode, kindly turn off any other sensors.

          ```C
          SL_SH_ADC_PS1=1 
          //Enabling this macro will move the core from PS2 Active state to PS1 state by using the Power_Task 

          //PATH:\gecko_sdk_4.3 2\util\third_party\freertos\kernel\include\FreeRTOS.h
          #ifndef configUSE_TICKLESS_IDLE
          #define configUSE_TICKLESS_IDLE 0 // 0 is to Disable the tickless Idle mode 
          #endif
          ```

## Sensor Pins Setup

### I2C Sensor Pin Configurations

| Sensor PIN | ULP GPIO PIN | Description |
| --- | --- | --- |
| SCL | ULP_GPIO_7 [EXP_HEADER-15] | Connect to SCL pin |
| SDA | ULP_GPIO_6 [EXP_HEADER-16] | Connect to SDA pin |
| VCC | (WPK) (3v3) | Connect to 3v3 pin |
| GND | (WPK) (GND) | Connect to GND pin |
| ADDR (for BH1750 Light Sensor) |(WPK) (GND) | Connect to GND pin |
|


### ADC Sensor Pin Configurations

| Sensor PIN | ULP GPIO PIN | Description |
| --- | --- | --- |
| ADC Input | ULP_GPIO_10 [ P17 ] | Connect to Joystick output (P36)
|

## Test the Application

- Compile and run the application.
- Connect the I2C and ADC sensors, based on the above pin configuration.

## Expected Results

- The sensor events should be observed on the Serial Terminal as per the given configuration. 

>### Note:
>#### General
  >
  >- ADC sensor will not work in Sleep state.
  >- The GPIO bsed Interrupt Sensor Mode won't function in Sleep mode.
  >
>#### ADC
  >
  >- ADC static mode will read the data from the ADC registers and does not depends on ADC Interrupt.
  >- ADC static mode only supports SensorHub's Polling mode. And it will read 1 sample at a time based on the Sensor Hub Polling Sampling interval
  >- ADC FIFO mode will support SensorHub Interrupt Mode.
