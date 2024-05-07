# SL ULP GPIO

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

- The ULP GPIO has 2 instances in MCU.
  - ULP Domain is used to control the ULP GPIO's(ULP_GPIO_n; n=0 to 11)
  - UULP Domain which is used to control the UULP GPIO's(UULP_GPIO_n; n=0 to 4)
- ULP GPIO domain has only one port and calling as Port 4 in program which has maximum of 12 pins.
- All the GPIO pins in ULP Domain support set,clear,toggle,programmed as output,input etc.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4338A]

### Software Requirements

- Simplicity Studio
- Serial console Setup
  - The Serial Console setup instructions are provided below:
Refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-soc-mode#perform-console-output-and-input-for-brd4338-a).

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

- Details for the example code are described in the following sub-sections.

### Initialization of GPIO in ULP Domain

  > **Note:**
  >
  > GPIO ULP instance have Port-4.

- GPIO to work in ULP Domain requires few steps to consider.
- Call \ref ulp_gpio_initialization(). This API has some API's being called, which are discussed below.
  - Enable \ref sl_si91x_gpio_enable_clock(), passing enumerator ULPCLK_GPIO of type sl_si91x_gpio_select_clock_t as parameter.
  - Enable PAD receiver for GPIO pin to program if using pin as input \ref sl_si91x_gpio_enable_ulp_pad_receiver(), passing GPIO pin number as parameter.
  - Set mode of the GPIO pin using \ref sl_gpio_set_pin_mode() API. Pass port of type \ref sl_gpio_port_t, pin, mode of type \ref sl_gpio_mode_t, output value as parameters.
  - Set direction of the GPIO pin using \ref sl_si91x_gpio_set_pin_direction() API. Pass port, pin , direction of type \ref sl_si91x_gpio_direction_t as parameters.
- We can use \ref sl_gpio_get_pin_input() API, to get/read status from gpio pin. Parameters passed are ULP port and pin.
- To handle the pin interrupt in ULP GPIO, API \ref sl_si91x_gpio_configure_ulp_pin_interrupt() is used.
- To handle the group interrupt in ULP GPIO, API \ref sl_si91x_gpio_configure_ulp_group_interrupt() is used.

### Initialization of GPIO in UULP Domain

- GPIO to work in UULP Domain requires few steps to consider.
- Call \ref uulp_gpio_initialization(). This API has some API's being called, which are discussed below.
  - Enable \ref sl_si91x_gpio_enable_clock(), passing enumerator ULPCLK_GPIO of type sl_si91x_gpio_select_clock_t as parameter.
  - Enable input buffer for GPIO pin \ref sl_si91x_gpio_select_uulp_npss_input_buffer(), passing GPIO pin number, buffer of type \ref sl_si91x_gpio_input_buffer_t as parameter.
  - Set mode of the GPIO pin using \ref sl_si91x_gpio_set_uulp_npss_pin_mux() API. Pass pin, mode of type \ref sl_si91x_uulp_npss_mode_t as parameters.
  - Set direction of the GPIO pin using \ref sl_si91x_gpio_set_uulp_npss_direction() API. Pass pin , direction of type \ref sl_si91x_gpio_direction_t as parameters.
- We can use \ref sl_si91x_gpio_set_uulp_npss_pin_value() API, to set the GPIO pin.
- To handle the pin interrupt in UULP GPIO, API \ref sl_si91x_gpio_configure_uulp_interrupt() is used.
- To get the status of the GPIO pin use \ref sl_si91x_gpio_get_uulp_npss_pin() API, by passing pin number as parameter.


### Configuration and Steps for Execution

- Configure the following parameters in ulp_gpio_example.c (examples/si91x_soc/peripheral/sl_si91x_ulp_gpio/) file and update/modify following macros if required

  ```c
  #define PORT0                    0      // GPIO Port number(0 to 4)
  #define PIN_COUNT                2      // Number of interrupts needed
  #define POLARITY                 0      // Polarity for GPIO pin
  #define GRP_CNT                  2      // Count of group interrupt pins
  #define INT_CH                   0      // GPIO Pin interrupt 0
  #define NPSS_INTR                2      // NPSS GPIO interrupt number
  #define DELAY                    1000   // Delay for 1sec
  ```
  
- If UULP_GPIO_PIN is enabled, UULP pin direction is printed on the serial console. Connect logic analyzer to P14 on WPK board to observe the toggle state.

   ```c
     UULP_GPIO_PIN       = 1, // Initialize GPIO UULP instance
   ```
- If ULP_GPIO_PIN_INTR is enabled, it triggers ULP Domain pin interrupt. For analyzing pin interrupt keep a print (or) toggle (or) set(or) clear in ULP_PIN_IRQ_Handler() present in ulp_gpio_example.c.

   ```c
    ULP_GPIO_PIN_INTR    = 1, // Configure GPIO ULP instance pin interrupt
   ```

- If ULP_GPIO_GROUP_INTR is enabled, it triggers ULP group interrupt. For analyzing group interrupt keep a print (or) toggle (or) set(or) clear in ULP_GROUP_IRQ_Handler() present in ulp_gpio_example.c.

   ```c
    ULP_GPIO_GROUP_INTR = 1, // Configure GPIO ULP instance group interrupt
   ```

- If UULP_GPIO_PIN_INTR is enabled, it triggers UULP pin interrupt. For analyzing pin interrupt keep a print (or) toggle (or) set(or) clear in UULP_PIN_IRQ_Handler() present in ulp_gpio_example.c.

   ```c
    UULP_GPIO_PIN_INTR  = 1, // Configure GPIO UULP instance pin interrupt
   ```


## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

1. Compile and run the application.
2. By default ULP_GPIO_PIN(ULP GPIO instance) is enabled, ULP pin direction, mode are printed on the serial console. Connect ULP_GPIO_1 pin to 0v and to 3.3v, and observe the LED0 toggle state. By default led(LED0) will be in high state. Connect logic analyzer to P16(ULP_GPIO_1), F10(LED0) and observe the pins state.

NOTE: These pin configurations are specific to BRD4338A board.
- ALL enumerators defined below are of type \ref gpio_instance_type_t which are present in ulp_gpio_example.c. Make corresponding enumerator to '1', in order to enable the individual functionalities mentioned below.
- By default ULP_GPIO_PIN(ULP GPIO instance) is enabled, ULP pin direction, mode are printed on the serial console. Connect ULP_GPIO_1 pin to 0v and to 3.3v, and observe the LED0 toggle state. By default led(LED0) will be in high state. Connect logic analyzer to P16(ULP_GPIO_1), F10(LED0) and observe the pins state.
- After successful program execution the prints in serial console looks as shown below.

  >![output](resources/readme/output_ulp_gpio.png)

**Note:**
>- The required files for low power state are moved to RAM rest of the application is executed from flash.
>- In this application we are changing the power state from PS4 to PS2 and vice - versa. 

