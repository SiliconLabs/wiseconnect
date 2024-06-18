# SiWx917 Software Reference

## Overview

This reference manual provides information about the software on the SiWx917™, the first chip in the SiWx91x™ chipset family. It is intended to provide all the details required for a smooth developer experience.

The SiWx91x is the industry's first wireless microcontroller unit (MCU) family with a comprehensive multi-protocol wireless subsystem. It has an integrated ultra low power microcontroller, a built-in wireless subsystem, advanced security, high performance mixed-signal peripherals, and integrated power management.

The SiWx917 system-on-chip (SoC) has two processing cores:
- ThreadArch™ (TA) wireless connectivity processor, also known as the network processor (**NWP**)
- ARM Cortex M4 application processor, also known as the **MCU**

The SiWx917 operates either with two flash memories, one for the MCU and the other for the NWP, or with a shared flash memory. There is a shared SRAM used by both the processors. The NWP provides support for in-built network and wireless stacks. These are accessed by an application running on the MCU via pre-defined APIs provided as part of the WiSeConnect™ SDK. Both the processors are connected over an AHB interface. Functioning in both the MCU and NWP is achieved via state machines present in the firmware. 

MCU applications can be developed, compiled, and run on the SiWx917 using the WiSeConnect SDK v3.x extension (or **WiSeconnect 3** extension) on Simplicity Studio. NWP firmware is available as a pre-built binary with the WiSeConnect SDK package. See the [Getting Started](http://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) documentation for more details.

>**Note:** RTOS support is available at Application and Service level. Applications developers should ensure to use right RTOS primitives when accessing MCU peripherals from multiple SW threads.When using RTOS, need to configure interrupt priorities for all MCU interrupts being used in the application.

## Software Architecture

This section provides an overview of the SiWx917 software components. The SiWx917 firmware consists of the MCU application and NWP wireless firmware. The MCU application uses the [WiSeConnect SDK](http://github.com/siliconlabs/wiseconnect) which includes source code in C and example applications with cloud SDKs, sensor-hub, MCU peripherals, Wi-Fi, BLE, and low-level MCU drivers.

The following diagram illustrates the software architecture of the SiWx917:

![Software Architecture Diagram](./resources/Si917.png)

### SiWx91x MCU

The ARM Cortex M4 application processor (MCU) on the SiWx917 is a high performance, 32-bit processor which can operate up to 180 MHz. 

The MCU provides a rich set of core peripherals such as the systick timer, FPU, NVIC, etc.

The WiSeConnect SDK provides CMSIS supported drivers for a few peripherals while the remaining are supported by non-CMSIS drivers.

The following diagram shows CMSIS and non-CMSIS peripherals in different blocks. It also higlights the ultra low power (ULP) peripherals.

For more details on MCU Peripherals, see the **SiWx917 Reference Manual** (contact [sales](https://www.silabs.com/about-us/contact-sales) for access). 

![SiWx917 MCU Architecture](./resources/SiWx917_mcu_architecture.PNG)

### Bootup Flow

This section describes the SiWx917's software execution flow from bootup until the MCU application starts running.

SiWx917 has two bootloaders - the security bootloader and the application bootloader. The security bootloader is part of TASS ROM and the application bootloader is part of M4SS ROM. Secure Boot is implemented in the security bootloader.

On bootup, the security bootloader:
- Configures the module hardware based on the configuration present in the e-fuse and flash memory. 
- Authenticates the flash configuration settings.
- Passes the required information from the e-fuse to the application bootloader.
- Uses public and private key based digital signatures to authenticate the firmware images.
- Invokes the application bootloader.

![TA and M4 bootloader flow](./resources/boot_up_flow.png)

## WiSeConnect SDK Programming Model

### Application Flow

The following diagram demonstrates the flow of code in an SiWx917 MCU application code created using the WiSeConnect SDK.

Code blocks in green represent SiWx91x MCU APIs and code blocks on orange represent Wireless APIs. See the [WiSeConnect API Reference Guide](http://docs.silabs.com/wiseconnect/latest/wiseconnect-api-reference-guide-summary/) for more information.

![Application Flow](./resources/application_flow.png)

### Clock Configuration

The SiWx917 clock subsystem facilitates changing the clock source and/or frequency for different functionalities to fine tune the power usage and performance of a given application. The subsystems supports configuring on-chip clocks such as ULP clock oscillators and high-freqeuency PLLs, or clocks to processor, high speed interfaces, and peripherals (including MCU HP, MCU ULP and UULP Vbat).

* Multiple high frequency clocks generated by PLLs
  * High Frequency Clock from 1MHz - 180MHz (SOC_PLL_CLK)
  * High Frequency Interface Clock from 1MHz - 180MHz (INTF_PLL_CLK)
  * Defined frequencies for I2S Interface (I2S_PLL_CLK)
* Multiple clocks generated by ULP Clock Oscillators. These are low-power clock oscillators
  * External Crystal clock (XTAL_CLK)
  * RC 32MHz Clock (RC_32MHZ_CLK)
  * RO High-Frequency clock (RO_HF_CLK)
  * Doubler Clock (DOUBLER_CLK)
  * RC 32kHz Clock (RC_32KHZ_CLK)
  * RO 32kHz Clock (RO_32KHZ_CLK)
  * XTAL 32kHz clock (XTAL_32KHZ_CLK)
* Configurable independent division factors for varying the frequencies of different functional blocks
* Configurable independent clock gating for different functional blocks

By deafult the MCU clock is configured to 40MHz using the **XTAL_CLK** clock source.

The following example code snippet illustrates setting the SOCPLL clock (1 MHz to 180 MHz) as MCUSOC-Clock:

```C
#include "sl_si91x_clock_manager.h"

#define SOC_PLL_CLK  ((uint32_t)(180000000)) // 180MHz default SoC PLL Clock as source to Processor

// Core Clock runs at 180MHz SOC PLL Clock
sl_si91x_clock_manager_m4_set_core_clk(M4_SOCPLLCLK, SOC_PLL_CLK);

```
> **Note**
1. **Clock Manager** component needs to be installed to use this function
2. For reference, look into example applications where the MCU clock is reconfigured to 180MHz in the application using the function **default_clock_configuration();**

### GPIO Configuration

The SiWx917 has totally 53 general-purpose input-output (GPIO) ports. The number of GPIOs available varies between different packages. Please refer to the **GPIO available vs package** table in the product datasheet for more details. Registers for GPIO pins that are not available on the package are reserved.

For configuring any GPIO on SiWx917 SoC, it is required to program the following:

* Pad Configuration
* Pin Muxing

#### Pad Configuration

There is a common set of GPIO pads shared by multiple processor subsystems containing the secure zone processor (SZP), MCU high performance (HP), and MCU ultra low power (ULP) subsystems. 

It is possible to control GPIO pads from either the SZP, MCU HP, or MCU ULP. The pad selection register has to be programmed to control the GPIOs between the subsystems. 

The following registers may be configured in order to access any of the GPIOs to MCU HP:
* Pad selection Register
* Pad configuration Register
* GPIO mode Register

Refer to the **SiWx917 Pad Configurations** section in the **SiWx917 Reference Manual** for details (contact [sales](https://www.silabs.com/about-us/contact-sales) for access).

#### Pin Muxing

The SiWx917 provides multiplexing features on several pins. It is possible to configure SoC GPIOs as ULP GPIOs and vice versa to achieve desired pin functionality. 

There are many digital and analog peripherals on the SiWx917 such as the I2C, I2S, UART, SPI, Ethernet, SDIO, SDMEM, PWM, QEI, CAN, etc. However, the number of pins is limited. 

The pin multiplexing module makes it possible to accommodate all of the peripherals in a small package without compromising on functionality. The module makes it possible to multiplex different functions on the same I/O pin. If a I/O pin is used for a peripheral function, it cannot be used as GPIO. The reset values for the GPIO mode for each of the GPIOs are provided in the **SiWx917 Reference Manual** (contact [sales](https://www.silabs.com/about-us/contact-sales) for access) under **PAD Configuration and GPIO Mode Reset Values** section  .

Refer to the **SiWx917 Pin MUX** section in the **SiWx917 Reference Manual** for details (contact [sales](https://www.silabs.com/about-us/contact-sales) for access).

**Code Snippet - GPIO Toggle**

```C
#define PORT        0
#define GPIO_PIN    GPIO_6
#define PAD_NUM     1
   
//!Enable pad section bit for GPIO 6
sl_si91x_gpio_enable_pad_selection(PAD_NUM);
  
//!Set GPIO mode to mode 0
sl_gpio_set_pin_mode(PORT, GPIO_PIN, EGPIO_PIN_MUX_MODE0,0);

//!Set GPIO to output direction 
sl_si91x_gpio_set_pin_direction(PORT, GPIO_PIN, EGPIO_CONFIG_DIR_OUTPUT);

//!Set GPIO pin value to 0
sl_gpio_clear_pin_output(PORT, GPIO_PIN);
     
//!Set GPIO pin value to 1
sl_gpio_set_pin_output(PORT, GPIO_PIN);
```

> **Note**: Please see the [Si91x - SL_GPIO](https://github.com/SiliconLabs/wiseconnect/tree/master/examples/si91x_soc/peripheral/sl_si91x_gpio) example for more information.

**Code Snippet - Configuring GPIO In Peripheral Mode**

```C
/* Example for configuring GPIO6 in UART2_RX(mode 6) */
#define PORT        0
#define GPIO_PIN    GPIO_6
#define PAD_NUM     1
   
//!Enable pad section bit for GPIO 6
sl_si91x_gpio_enable_pad_selection(PAD_NUM);
  
//!Receiver Enable for RX pin 
sl_si91x_gpio_enable_pad_receiver(GPIO_PIN);

//!Set GPIO mode to mode 0
sl_gpio_set_pin_mode(PORT, GPIO_PIN, EGPIO_PIN_MUX_MODE6,0);
```

##### SoC GPIOs

The SoC GPIOs below (GPIO_6 to GPIO_51) are available in the normal mode of operation (power states 3 and 4). For a description of power states, see the **SiWx917 Reference Manual** (contact [sales](https://www.silabs.com/about-us/contact-sales) for access). 

Each of these GPIO pin functions is controlled by the GPIO Mode register mentioned in **SoC GPIO's** section of the **SiWx917 Reference Manual** for details (contact [sales](https://www.silabs.com/about-us/contact-sales) for access). 

| **PIN** | **PAD NUMBER** | **GPIO** | **GPIO Mode= 0** | **GPIO Mode= 1** | **GPIO Mode= 2** | **GPIO Mode= 3** | **GPIO Mode= 4** | **GPIO Mode= 5** | **GPIO Mode= 6** | **GPIO Mode= 7** | **GPIO Mode= 8** | **GPIO Mode= 9** | **GPIO Mode= 10** | **GPIO Mode= 11** | **GPIO Mode= 12** | **GPIO Mode*= 13** | **GPIO Mode*= 14** | **GPIO Mode*= 15** |
| ------- | -------------- | ---------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- | -------------------------- | -------------------------- | -------------------------- | -------------------------- | -------------------------- | -------------------------- |
| 6 | 1 | GPIO_6 | GPIO_6 | SIO_0 | USART1_CTS | SSI_MST_DATA2 | I2C1_SDA | I2C2_SCL | UART2_RX | I2S_2CH_DIN_1 | PMU_TEST_1 | ULPPERH_ON_SOC_GPIO_0 | PWM_1L | M4SS_QSPI_D0 | GSPI_MST1_MOS1 | M4SS_TRACE_CLKIN |  | NWP_GPIO_6 |
| 7 | 2 | GPIO_7 | GPIO_7 | SIO_1 | USART1_DTR | SSI_MST_DATA3 | I2C1_SCL | I2C2_SDA | UART2_TX | I2S_2CH_DOUT_1 | PMU_TEST_2 | ULPPERH_ON_SOC_GPIO_1 | PWM_1H | M4SS_QSPI_CSN0 | M4SS_QSPI_CSN1 | M4SS_TRACE_CLK |  |  |
| 8 | 3 | GPIO_8 | GPIO_8 | SIO_2 | USART1_CLK | SSI_MST_CLK | GSPI_MST1_CLK | QEI_IDX | UART2_RS485_RE | I2S_2CH_CLK | SSI_SLV_CLK | ULPPERH_ON_SOC_GPIO_2 | PWM_2L | M4SS_QSPI_CLK | SCT_OUT_2 | M4SS_TRACE_D0 |  | NWP_GPIO_8 |
| 9 | 4 | GPIO_9 | GPIO_9 | SIO_3 | USART1_RTS | SSI_MST_CS0 | GSPI_MST1_CS0 | QEI_PHA | UART2_RS485_DE | I2S_2CH_WS | SSI_SLV_CS |  | PWM_2H | M4SS_QSPI_D1 | SCT_OUT_3 | M4SS_TRACE_D1 |  | NWP_GPIO_9 |
| 10 | 5 | GPIO_10 | GPIO_10 | SIO_4 | USART1_RX | SSI_MST_CS1 | GSPI_MST1_CS1 | QEI_PHB | UART2_RTS | I2S_2CH_DIN_0 | SSI_SLV_MOSI | ULPPERH_ON_SOC_GPIO_4 | PWM_3L | M4SS_QSPI_D2 | SSI_MST_DATA1 | M4SS_TRACE_D2 |  | NWP_GPIO_10 |
| 11 | 6 | GPIO_11 | GPIO_11 | SIO_5 | USART1_DSR | SSI_MST_DATA0 | GSPI_MST1_MISO | QEI_DIR | UART2_CTS | I2S_2CH_DOUT_0 | SSI_SLV_MISO | ULPPERH_ON_SOC_GPIO_5 | PWM_3H | M4SS_QSPI_D3 | MCU_CLK_OUT | M4SS_TRACE_D3 |  | NWP_GPIO_11 |
| 12 | 7 | GPIO_12 | GPIO_12 |  | USART1_DCD | SSI_MST_DATA1 | GSPI_MST1_MOSI |  | UART2_RS485_EN |  | MCU_CLK_OUT | ULPPERH_ON_SOC_GPIO_6 | PWM_4L |  |  |  |  | NWP_GPIO_12 |
| 15 | 8 | GPIO_15 | GPIO_15 | SIO_7 | UART2_TX | SSI_MST_CS2 | GSPI_MST1_CS2 |  | M4SS_TRACE_CLKIN |  | MCU_CLK_OUT | ULPPERH_ON_SOC_GPIO_7 | PWM_4H |  |  |  |  | NWP_GPIO_15 |
| 25 | No need to select the Pad | GPIO_25 | GPIO_25 | SIO_0 | USART1_CLK | SSI_MST_CLK | GSPI_MST1_CLK | QEI_IDX |  | I2S_2CH_CLK | SSI_SLV_CS | SCT_IN_0 | PWM_FAULTA | ULPPERH_ON_SOC_GPIO_6 | SOC_PLL_CLOCK | USART1_IR_RX | TopGPIO_0 |  |
| 26 | No need to select the Pad | GPIO_26 | GPIO_26 | SIO_1 | USART1_CTS | SSI_MST_DATA0 | GSPI_MST1_MISO | QEI_PHA | UART2_RS485_EN | I2S_2CH_WS | SSI_SLV_CLK | SCT_IN_1 | PWM_FAULTB | ULPPERH_ON_SOC_GPIO_7 | INTERFACE_PLL_CLOCK | USART1_IR_TX | TopGPIO_1 |  |
| 27 | No need to select the Pad | GPIO_27 | GPIO_27 | SIO_2 | USART1_RI | SSI_MST_DATA1 | GSPI_MST1_MOSI | QEI_PHB | UART2_RTS | I2S_2CH_DIN_0 | SSI_SLV_MOSI | SCT_IN_2 | PWM_TMR_EXT_TRIG_1 | ULPPERH_ON_SOC_GPIO_8 | I2S_PLL_CLOCK | USART1_RS485_EN | TopGPIO_2 |  |
| 28 | No need to select the Pad | GPIO_28 | GPIO_28 | SIO_3 | USART1_RTS | SSI_MST_CS0 | GSPI_MST1_CS0 | QEI_DIR | UART2_RTS | I2S_2CH_DOUT_0 | SSI_SLV_MISO | SCT_IN_3 | PWM_TMR_EXT_TRIG_2 | ULPPERH_ON_SOC_GPIO_9 | XTAL_ON_IN | USART1_RS485_RE | TopGPIO_3 |  |
| 29 | No need to select the Pad | GPIO_29 | GPIO_29 | SIO_4 | USART1_RX | SSI_MST_DATA2 | GSPI_MST1_CS1 | I2C2_SCL | UART2_RX | I2S_2CH_DIN_1 | PMU_TEST_1 | SCT_OUT_0 | PWM_TMR_EXT_TRIG_3 | ULPPERH_ON_SOC_GPIO_10 | USART1_DCD | USART1_RS485_DE | TopGPIO_4 |  |
| 30 | No need to select the Pad | GPIO_30 | GPIO_30 | SIO_5 | USART1_TX | SSI_MST_DATA3 | GSPI_MST1_CS2 | I2C2_SDA | UART2_TX | I2S_2CH_DOUT_1 | PMU_TEST_2 | SCT_OUT_1 | PWM_TMR_EXT_TRIG_4 | ULPPERH_ON_SOC_GPIO_11 | PMU_TEST_1 | PMU_TEST_2 | TopGPIO_5 |  |
| 31 | 9 | GPIO_31 | GPIO_31 |  |  |  |  |  |  |  |  |  |  | I2C1_SDA | UART2_RTS | QEI_IDX |  |  |
| 32 | 9 | GPIO_32 | GPIO_32 |  |  |  |  |  |  |  |  |  |  | I2C1_SCL | UART2_CTS | QEI_PHA |  |  |
| 33 | 9 | GPIO_33 | GPIO_33 |  |  |  |  |  |  |  |  |  |  | I2C2_SCL | UART2_RX | QEI_PHB |  |  |
| 34 | 9 | GPIO_34 | GPIO_34 |  |  |  |  |  |  |  |  |  |  | I2C2_SDA | UART2_TX | QEI_DIR |  |  |
| 46 | 10 | GPIO_46 | GPIO_46 | M4SS_QSPI_CLK | USART1_RI | QEI_IDX | GSPI_MST1_CLK |  | M4SS_TRACE_CLKIN | I2S_2CH_CLK | SSI_SLV_CS | ULPPERH_ON_SOC_GPIO_8 | SOC_PLL_CLOCK | M4SS_PSRAM_CLK |  |  |  | NWP_GPIO_46 |
| 47 | 11 | GPIO_47 | GPIO_47 | M4SS_QSPI_D0 | USART1_IR_RX | QEI_PHA | GSPI_MST1_MISO |  | M4SS_TRACE_CLK | I2S_2CH_WS | SSI_SLV_CLK | ULPPERH_ON_SOC_GPIO_9 | INTERFACE_PLL_CLOCK | M4SS_PSRAM_D0 |  |  |  | NWP_GPIO_47 |
| 48 | 12 | GPIO_48 | GPIO_48 | M4SS_QSPI_D1 | USART1_IR_TX | QEI_PHB | GSPI_MST1_MOSI |  | M4SS_TRACE_D0 | I2S_2CH_DIN_0 | SSI_SLV_MOSI | ULPPERH_ON_SOC_GPIO_10 | I2S_PLL_CLOCK | M4SS_PSRAM_D1 |  |  |  | NWP_GPIO_48 |
| 49 | 13 | GPIO_49 | GPIO_49 | M4SS_QSPI_CSN0 | USART1_RS485_EN | QEI_DIR | GSPI_MST1_CS0 |  | M4SS_TRACE_D1 | I2S_2CH_DOUT_0 | SSI_SLV_MISO | ULPPERH_ON_SOC_GPIO_11 | MCU_QSPI_CSN0 | M4SS_PSRAM_CSN0 |  |  |  | NWP_GPIO_49 |
| 50 | 14 | GPIO_50 | GPIO_50 | M4SS_QSPI_D2 | USART1_RS485_RE | SSI_MST_CS2 | GSPI_MST1_CS1 | I2C2_SCL | M4SS_TRACE_D2 | I2S_2CH_DIN_1 | PWM_TMR_EXT_TRIG_4 | UART2_RTS | MEMS_REF_CLOCK | M4SS_PSRAM_D2 |  |  |  | NWP_GPIO_50 |
| 51 | 15 | GPIO_51 | GPIO_51 | M4SS_QSPI_D3 | USART1_RS485_DE | SSI_MST_CS3 | GSPI_MST1_CS2 | I2C2_SDA | M4SS_TRACE_D3 | I2S_2CH_DOUT_1 | PWM_TMR_EXT_TRIG_1 | UART2_CTS | PLL_TESTMODE_SIG | M4SS_PSRAM_D3 |  |  |  | NWP_GPIO_51 |
| 52 | 16 | GPIO_52 | GPIO_52 |  | USART1_CLK | SSI_MST_CLK | GSPI_MST1_CLK | QEI_IDX | M4SS_TRACE_CLKIN | I2S_2CH_CLK | SSI_SLV_CLK | M4SS_QSPI_CLK | SOC_PLL_CLOCK |  | M4SS_PSRAM_CLK |  |  |  |
| 53 | 17 | GPIO_53 | GPIO_53 | M4SS_QSPI_CSN1 | USART1_RTS | SSI_MST_CS0 | GSPI_MST1_CS0 | QEI_PHA | M4SS_TRACE_CLK | I2S_2CH_WS | SSI_SLV_CS | M4SS_QSPI_D0 | INTERFACE_PLL_CLOCK | M4SS_PSRAM_CSN1 | M4SS_PSRAM_D0 |  |  |  |
| 54 | 18 | GPIO_54 | GPIO_54 | M4SS_QSPI_D4 | USART1_TX | SSI_MST_DATA2 | GSPI_MST1_CS1 | I2C2_SCL | M4SS_TRACE_D0 | I2S_2CH_DIN_1 | PWM_TMR_EXT_TRIG_2 | M4SS_QSPI_D1 | I2S_PLL_CLOCK | M4SS_PSRAM_D4 | M4SS_PSRAM_D1 |  |  |  |
| 55 | 19 | GPIO_55 | GPIO_55 | M4SS_QSPI_D5 | USART1_RX | SSI_MST_DATA3 | GSPI_MST1_CS2 | I2C2_SDA | M4SS_TRACE_D1 | I2S_2CH_DOUT_1 | PWM_TMR_EXT_TRIG_3 | M4SS_QSPI_CSN0 |  | M4SS_PSRAM_D5 | M4SS_PSRAM_CSN0 |  |  |  |
| 56 | 20 | GPIO_56 | GPIO_56 | M4SS_QSPI_D6 | USART1_CTS | SSI_MST_DATA0 | GSPI_MST1_MISO | QEI_PHB | M4SS_TRACE_D2 | I2S_2CH_DIN_0 | SSI_SLV_MOSI | M4SS_QSPI_D2 | MEMS_REF_CLOCK | M4SS_PSRAM_D6 | M4SS_PSRAM_D2 |  |  |  |
| 57 | 21 | GPIO_57 | GPIO_57 | M4SS_QSPI_D7 | USART1_DSR | SSI_MST_DATA1 | GSPI_MST1_MOSI | QEI_DIR | M4SS_TRACE_D3 | I2S_2CH_DOUT_0 | SSI_SLV_MISO | M4SS_QSPI_D3 | XTAL_ON_IN | M4SS_PSRAM_D7 | M4SS_PSRAM_D3 |  |  |  |

The following WiSeConnect SDK APIs are available for configuring the SoC GPIOs:
- `sl_si91x_gpio_enable_pad_selection(PIN)` to enable the control of GPIOs by the MCU.
- `sl_gpio_set_pin_mode(PORT0, PIN, MODE, OUTPUT_VALUE)` to set the mode of a pin to use the SoC GPIO.
- `sl_si91x_gpio_set_pin_direction(PORT0, PIN, DIR)` to set the pin direction. For example, if the mode is configured as 0 using the `sl_gpio_set_pin_mode()` API, the pin direction is 0 for output and 1 for input.
- `sl_si91x_gpio_enable_pad_receiver(PIN)` to enable pad receiving if the pin is an input.

The following is an example code snippet illustrating the use of the above APIs:

```C
   /*Enable PAD for GPIO_10*/
   sl_si91x_gpio_enable_pad_selection(5);

   /*Configuring GPIO_10 MODE as GPIO */
   sl_gpio_set_pin_mode(0, 10, 0, 1);

   /*Set GPIO_10 direction as OUTPUT*/
   sl_si91x_gpio_set_pin_direction(0, 10, 0);
```

##### SDIO Host Interfaces

|GPIO    |  GPIO Default State SPI Mode|
|GPIO_25 |  SDIO_CLK |
|GPIO_26 |  SDIO_CMD |
|GPIO_27 |  SDIO_D0 |
|GPIO_28 |  SDIO_D1 |
|GPIO_29 |  SDIO_D2 |
|GPIO_30 |  SDIO_D3 |

##### Digital ULP GPIOs

The SoC GPIOs configured for ULP Peripheral functionality (ULPPERH_ON_SOC_GPIO_0 to ULPPERH_ON_SOC_GPIO_11) are available only in the normal mode of operation (Power-states 4 and 3). For a description of power-states,For a description of power states, see the **SiWx917 Reference Manual** (contact [sales](https://www.silabs.com/about-us/contact-sales) for access).

Each of these GPIO pin functions is controlled by the Special Function ULP register mentioned in **ULP GPIO's** section of the **SiWx917 Reference Manual** (contact [sales](https://www.silabs.com/about-us/contact-sales) for access).

> **Note:** These Digital functions are only available in normal mode of operation PS3 and PS4. For more details refer to powersave app note

| PIN | ULP_GPIO | ULP_GPIO Mode 0 | ULP_GPIO Mode 1 | ULP_GPIO Mode 2 | ULP_GPIO Mode 3 | ULP_GPIO Mode 4 | ULP_GPIO Mode 5 | ULP_GPIO Mode 6 | ULP_GPIO Mode 7 | ULP_GPIO Mode 8 | ULP_GPIO Mode 9 | ULP_GPIO Mode 10 | ULP_GPIO Mode 11 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0 | ULPPERH_ON_SOC_GPIO_0 | ULP_EGPIO[0] | ULP_SPI_CLK | ULP_I2S_DIN | ULP_UART_RTS | ULP_I2C_SDA |  |  |  |  |  |  |  |
| 1 | ULPPERH_ON_SOC_GPIO_1 | ULP_EGPIO[1] | ULP_SPI_DOUT | ULP_I2S_DOUT | ULP_UART_CTS | ULP_I2C_SCL | Timer0 |  |  |  |  |  |  |
| 2 | ULPPERH_ON_SOC_GPIO_2 | ULP_EGPIO[2] | ULP_SPI_DIN | ULP_I2S_WS | ULP_UART_RX |  | COMP1_OUT |  |  |  |  |  |  |
| 4 | ULPPERH_ON_SOC_GPIO_4 | ULP_EGPIO[4] | ULP_SPI_CS1 | ULP_I2S_WS | ULP_UART_RTS | ULP_I2C_SDA |  | NPSS_TEST_MODE_1 |  | ULP_SPI_CLK | Timer0 | IR_INPUT |  |
| 5 | ULPPERH_ON_SOC_GPIO_5 | ULP_EGPIO[5] | IR_OUTPUT | ULP_I2S_DOUT | ULP_UART_CTS | ULP_I2C_SCL | AUX_ULP_TRIG_0 | NPSS_TEST_MODE_2 |  | ULP_SPI_DOUT | Timer1 | IR_OUTPUT |  |
| 6 | ULPPERH_ON_SOC_GPIO_6 | ULP_EGPIO[6] | ULP_SPI_CS2 | ULP_I2S_DIN | ULP_UART_RX | ULP_I2C_SDA |  |  |  | ULP_SPI_DIN | COMP1_OUT | AUX_ULP_TRIG_0 |  |
| 7 | ULPPERH_ON_SOC_GPIO_7 | ULP_EGPIO[7] | IR_INPUT | ULP_I2S_CLK | ULP_UART_TX | ULP_I2C_SCL | Timer1 |  |  | ULP_SPI_CS0 | COMP2_OUT | AUX_ULP_TRIG_1 | NPSS_TEST_MODE_0 |
| 8 | ULPPERH_ON_SOC_GPIO_8 | ULP_EGPIO[8] | ULP_SPI_CLK | ULP_I2S_CLK | ULP_UART_CTS | ULP_I2C_SCL | Timer0 |  |  |  |  |  |  |
| 9 | ULPPERH_ON_SOC_GPIO_9 | ULP_EGPIO[9] | ULP_SPI_DIN | ULP_I2S_DIN | ULP_UART_RX | ULP_I2C_SDA | COMP1_OUT |  |  |  |  |  |  |
| 10 | ULPPERH_ON_SOC_GPIO_10 | ULP_EGPIO[10] | ULP_SPI_CS0 | ULP_I2S_WS | ULP_UART_RTS | IR_INPUT |  | NPSS_TEST_MODE_0 |  |  |  |  |  |
| 11 | ULPPERH_ON_SOC_GPIO_11 | ULP_EGPIO[11] | ULP_SPI_DOUT | ULP_I2S_DOUT | ULP_UART_TX | ULP_I2C_SDA | AUX_ULP_TRIG_0 |  |  |  |  |  |  |

```C
/* Example for configuring GPIO10 in UART2_RX(mode 6) */
#define PORT        0
#define GPIO_PIN    PIN10
#define PAD_NUM     2

// Enable M4 Clock​
sl_si91x_gpio_enable_clock((sl_si91x_gpio_select_clock_t)M4CLK_GPIO);​

// Enable pad selection for GPIO pin​
sl_si91x_gpio_enable_pad_selection(PAD_NUM);​

// Enable pad receiver for GPIO pin (For Input GPIO)​
sl_si91x_gpio_enable_pad_receiver(GPIO_PIN);​

// Set the pin mode for GPIO pin​
sl_gpio_set_pin_mode(PORT, GPIO_PIN, MODE9, OUTPUT_VALUE);​

// Select the Virtual ULP GPIO Mode​
sl_si91x_gpio_ulp_soc_mode((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO, ULP_GPIO_4, MODE3)​
```

##### ULP GPIO's

The ULP GPIOs listed in the table below (ULP_GPIO_0 to ULP_GPIO_11) are available in the normal mode of operation (power states 3 and 4) and also in the ultra low power mode of operation (power states 1 and 2). For a description of power states, refer the **SiWx917 Reference Manual** (contact [sales](https://www.silabs.com/about-us/contact-sales) for access).

Each of these GPIO's Pin function is controlled by the Special ULP register mentioned in **ULP GPIO's** section of the **SiWx917 Reference Manual** (contact [sales](https://www.silabs.com/about-us/contact-sales) for access).

| PIN | ULP_GPIO | ULP_GPIO Mode 0 | ULP_GPIO Mode 1 | ULP_GPIO Mode 2 | ULP_GPIO Mode 3 | ULP_GPIO Mode 4 | ULP_GPIO Mode 5 | ULP_GPIO Mode 6 | ULP_GPIO Mode 7 | ULP_GPIO Mode 8 | ULP_GPIO Mode 9 | ULP_GPIO Mode 10 | ULP_GPIO Mode 11 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0 | ULP_GPIO_0 | ULP_EGPIO[0] | ULP_SPI_CLK | ULP_I2S_DIN | ULP_UART_RTS | ULP_I2C_SDA |  | SOCPERH_ON_ULP_GPIO_0 | AGPIO_0 |  |  |  |  |
| 1 | ULP_GPIO_1 | ULP_EGPIO[1] | ULP_SPI_DOUT | ULP_I2S_DOUT | ULP_UART_CTS | ULP_I2C_SCL | Timer2 | SOCPERH_ON_ULP_GPIO_1 | AGPIO_1 |  |  |  |  |
| 2 | ULP_GPIO_2 | ULP_EGPIO[2] | ULP_SPI_DIN | ULP_I2S_WS | ULP_UART_RX | NPSS_GPIO_4 | COMP1_OUT | SOCPERH_ON_ULP_GPIO_2 | AGPIO_2 |  |  |  |  |
| 4 | ULP_GPIO_4 | ULP_EGPIO[4] | ULP_SPI_CS1 | ULP_I2S_WS | ULP_UART_RTS | ULP_I2C_SDA | AUX_ULP_TRIG_1 | SOCPERH_ON_ULP_GPIO_4 | AGPIO_4 | ULP_SPI_CLK | Timer0 | IR_INPUT |  |
| 5 | ULP_GPIO_5 | ULP_EGPIO[5] | IR_OUTPUT | ULP_I2S_DOUT | ULP_UART_CTS | ULP_I2C_SCL | AUX_ULP_TRIG_0 | SOCPERH_ON_ULP_GPIO_5 | AGPIO_5 | ULP_SPI_DOUT | Timer1 | IR_OUTPUT |  |
| 6 | ULP_GPIO_6 | ULP_EGPIO[6] | ULP_SPI_CS2 | ULP_I2S_DIN | ULP_UART_RX | ULP_I2C_SDA |  | SOCPERH_ON_ULP_GPIO_6 | AGPIO_6 | ULP_SPI_DIN | COMP1_OUT | AUX_ULP_TRIG_0 |  |
| 7 | ULP_GPIO_7 | ULP_EGPIO[7] | IR_INPUT | ULP_I2S_CLK | ULP_UART_TX | ULP_I2C_SCL | Timer1 | SOCPERH_ON_ULP_GPIO_7 | AGPIO_7 | ULP_SPI_CS0 | COMP2_OUT | AUX_ULP_TRIG_1 | NPSS_TEST_MODE_0 |
| 8 | ULP_GPIO_8 | ULP_EGPIO[8] | ULP_SPI_CLK | ULP_I2S_CLK | ULP_UART_CTS | ULP_I2C_SCL | Timer0 | SOCPERH_ON_ULP_GPIO_8 | AGPIO_8 |  |  |  |  |
| 9 | ULP_GPIO_9 | ULP_EGPIO[9] | ULP_SPI_DIN | ULP_I2S_DIN | ULP_UART_RX | ULP_I2C_SDA | NPSS_TEST_MODE_0 | SOCPERH_ON_ULP_GPIO_9 | AGPIO_9 |  |  |  |  |
| 10 | ULP_GPIO_10 | ULP_EGPIO[10] | ULP_SPI_CS0 | ULP_I2S_WS | ULP_UART_RTS | IR_INPUT |  | SOCPERH_ON_ULP_GPIO_10 | AGPIO_10 |  |  |  |  |
| 11 | ULP_GPIO_11 | ULP_EGPIO[11] | ULP_SPI_DOUT | ULP_I2S_DOUT | ULP_UART_TX | ULP_I2C_SDA | AUX_ULP_TRIG_0 | SOCPERH_ON_ULP_GPIO_11 | AGPIO_11 |  |  |  |  |

The following WiSeConnect SDK APIs are available for configuring the ULP GPIOs:
- `sl_gpio_set_pin_mode(4, PIN, MODE, OUTPUT_VALUE)` to set the mode for a ULP GPIO.
- `sl_si91x_gpio_set_pin_direction(4, PIN, DIR)` to set the direction. For example, if the mode is configured as 0 using the `sl_gpio_set_pin_mode()` API, the direction is 0 for output and 1 for input.
- `sl_si91x_gpio_enable_ulp_pad_receiver(PIN)` to enable ULP pad receiving if the pin is an input.

The following is an example code snippet illustrating the use of the above APIs:

```C
   /* Configuring GPIO_10 MODE as GPIO */
   sl_gpio_set_pin_mode(4, 10, 0, 1);

   /* Set GPIO_10 direction as OUTPUT */
   sl_si91x_gpio_set_pin_direction(4, 10, 0);
```

##### Digital SoC GPIOs

The ULP GPIOs configured for SoC Peripheral functionality (SOCPERH_ON_ULP_GPIO_0 to SOCPERH_ON_ULP_GPIO_11) are available only in the normal mode of operation (Power-states 4 and 3). For a description of power-states, refer the **SiWx917 Reference Manual** (contact [sales](https://www.silabs.com/about-us/contact-sales) for access).

Each of these GPIO pin functions is controlled by the GPIO Mode register mentioned in **SoC GPIO's** section of the **SiWx917 Reference Manual** (contact [sales](https://www.silabs.com/about-us/contact-sales) for access).

> **Note:** These Digital functions are only available in normal mode of operation PS3 and PS4. For more details refer to powersave app note

| **PIN** | **PAD NUMBER** | **GPIO** | **GPIO Mode= 0** | **GPIO Mode= 1** | **GPIO Mode= 2** | **GPIO Mode= 3** | **GPIO Mode= 4** | **GPIO Mode= 5** | **GPIO Mode= 6** | **GPIO Mode= 7** | **GPIO Mode= 8** | **GPIO Mode= 9** | **GPIO Mode= 10** | **GPIO Mode= 11** | **GPIO Mode= 12** | **GPIO Mode*= 13** |
| ------- | -------------- | ---------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- | -------------------------- | -------------------------- | -------------------------- | -------------------------- |
| 64      | 22             | SOCPERH_ON_ULP_GPIO_0  | GPIO_64               | SIO_0                 | USART1_CLK            | QEI_IDX               | I2C1_SDA              | I2C2_SCL              | UART2_RS485_EN        | SCT_IN_0              | PWM_1L                | UART2_RTS             |                            | USART1_IR_RX               | PWM_1L                     | PMU_TEST_1                 |
| 65      | 23             | SOCPERH_ON_ULP_GPIO_1  | GPIO_65               | SIO_1                 | USART1_RX             | QEI_PHA               | I2C1_SCL              | I2C2_SDA              | UART2_RS485_RE        | SCT_IN_1              | PWM_1H                | UART2_CTS             |                            | USART1_IR_TX               | PWM_1H                     | PMU_TEST_2                 |
| 66      | 24             | SOCPERH_ON_ULP_GPIO_2  | GPIO_66               | SIO_2                 |                       | QEI_PHB               | I2C1_SCL              | I2C2_SCL              | UART2_RS485_DE        | SCT_IN_2              | PWM_2L                | UART2_RX              | PMU_TEST_1                 |                            |                            |                            |
| 67      | 25             | SOCPERH_ON_ULP_GPIO_3  | GPIO_67               | SIO_3                 |                       | QEI_DIR               | I2C1_SDA              | I2C2_SDA              |                       | SCT_IN_3              | PWM_2H                | UART2_TX              | PMU_TEST_2                 |                            |                            |                            |
| 68      | 26             | SOCPERH_ON_ULP_GPIO_4  | GPIO_68               | SIO_4                 | USART1_TX             | QEI_IDX               |                       |                       | UART2_RX              | SCT_OUT_0             | PWM_3L                | SCT_IN_0              | PWM_FAULTA                 | USART1_RI                  | PWM_2L                     | SCT_OUT_4                  |
| 69      | 27             | SOCPERH_ON_ULP_GPIO_5  | GPIO_69               | SIO_5                 | USART1_RTS            | QEI_PHA               |                       |                       | UART2_TX              | SCT_OUT_1             | PWM_3H                | SCT_IN_1              | PWM_FAULTB                 | USART1_RS485_EN            | PWM_2H                     | SCT_OUT_5                  |
| 70      | 28             | SOCPERH_ON_ULP_GPIO_6  | GPIO_70               | SIO_6                 | USART1_CTS            | QEI_PHB               | USART1_RX             | I2C2_SCL              | UART2_RTS             | SCT_OUT_2             | PWM_4L                | SCT_IN_2              | PWM_TMR_EXT_TRIG_1         | USART1_RS485_RE            | PMU_TEST_1                 | SCT_OUT_6                  |
| 71      | 29             | SOCPERH_ON_ULP_GPIO_7  | GPIO_71               | SIO_7                 | USART1_IR_RX          | QEI_DIR               | USART1_TX             | I2C2_SDA              | UART2_CTS             | SCT_OUT_3             | PWM_4H                | SCT_IN_3              | PWM_TMR_EXT_TRIG_2         | USART1_RS485_DE            | PMU_TEST_2                 | SCT_OUT_7                  |
| 72      | 30             | SOCPERH_ON_ULP_GPIO_8  | GPIO_72               | SIO_0                 | USART1_IR_TX          | QEI_IDX               |                       |                       | UART2_RX              | SCT_OUT_4             | PWM_SLP_EVENT_TRIG    | UART2_RTS             | PWM_TMR_EXT_TRIG_3         |                            |                            |                            |
| 73      | 31             | SOCPERH_ON_ULP_GPIO_9  | GPIO_73               | SIO_1                 | USART1_RS485_EN       | QEI_PHA               |                       |                       | UART2_TX              | SCT_OUT_5             | PWM_FAULTA            | UART2_CTS             | PWM_TMR_EXT_TRIG_4         |                            |                            |                            |
| 74      | 32             | SOCPERH_ON_ULP_GPIO_10 | GPIO_74               | SIO_2                 | USART1_RS485_RE       | QEI_PHB               | I2C1_SDA              |                       | UART2_RS485_RE        | SCT_OUT_6             | PWM_FAULTB            | UART2_RX              | PMU_TEST_1                 |                            |                            |                            |
| 75      | 33             | SOCPERH_ON_ULP_GPIO_11 | GPIO_75               | SIO_3                 | USART1_RS485_DE       | QEI_DIR               | I2C1_SCL              |                       | UART2_RS485_DE        | SCT_OUT_7             | PWM_TMR_EXT_TRIG_1    | UART2_TX              | PMU_TEST_2                 |                            |                            |                            |

```C
// Enable GPIO ULP_CLK​
sl_si91x_gpio_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO);​

// Enable pad receiver for ULP GPIO pin​
sl_si91x_gpio_enable_ulp_pad_receiver(ULP_PIN_8);​

// Set the pin mode for ULP GPIO pin​
sl_gpio_set_pin_mode(SL_ULP_GPIO_PORT, ULP_PIN_8, MODE6, OUTPUT_VALUE);​

// Enable pad selection for SoC GPIO pin​
sl_si91x_gpio_enable_pad_selection(30);​

// Set the pin mode for Virtual SoC GPIO pin​
sl_gpio_set_pin_mode(PORT0, PIN72, MODE1, OUTPUT_VALUE);
```

### User callback recommendation
- In RTOS environment, prefer Signaling mechanisms (Semaphore/Mutex/EventFlag etc.) instead of "Variables/Flags" from user callbacks to detect "*Transfer Complete*" for high speed communication peripherals

Refer below GSPI callback's sample code snippet demonstrating Mutex mechanism when RTOS is used:

```C
/*******************************************************************************
 * Callback event function
 * It is responsible for the event which are triggered by GSPI interface
 * It updates the respective member of the structure as the event is triggered.
 ******************************************************************************/
static void user_callback(uint32_t event)
{
  switch (event) {
    case SL_GSPI_TRANSFER_COMPLETE:
#if defined(SL_CATALOG_KERNEL_PRESENT)
      osMutexRelease(mutex_id);
#else
      gspi_transfer_complete = true;
#endif
      break;

    case SL_GSPI_DATA_LOST:
      break;

    case SL_GSPI_MODE_FAULT:
      break;
  }
}
```
### Power Save

The SiWx917 SoC supports sleep on both the MCU and NWP with or without RAM retention.

Sleep with RAM retention makes it possible for the configured SRAM banks to be retained across sleep and wake up. It is possible to select sleep without RAM retention if the retention of SRAM banks is not required.

Any peripherals (HP and ULP peripherals) that are initialized on powering up must be re-initialized at wake up.

#### Sleep Wakeup Sequence

**MCU Sleep Wakeup**

![ MCU Sleep Wakeup](./resources/MCU_sleep_wakeup.png)

**SoC Sleep Wakeup - Peripheral Interrupt Based Wakeup**

![ Peripheral wakeup](./resources/peripheral_wakeup.png)

**SoC Sleep Wakeup - Wireless Based Wakeup (Wake On Wireless)**

![ Wireless wakeup](./resources/wireless_wakeup.png)

#### MCU Wake Up Modes

The wake-up mode defines the bootloader sequence the SiWx917 will undergo once it comes out of sleep. The table below lists the different types of wake up modes available.

![ MCU wakeup modes](./resources/MCU_wakeup_modes.png)

#### Sequence to Configure MCU Power Save

1. Initialize the peripheral for the wakeup source.
    - Configure the wakeup source.
    - For example if it is a button based wakeup, then configure the GPIO button interrupt

    ```C
    //!Configure Wakeup-Source as wireless based 
    RSI_PS_SetWkpSources(WIRELESS_BASED_WAKEUP);
     ```

2. Set the retention size of the MCU RAM.
    - Possible retentions are
      - WISEMCU_16KB_RAM_IN_USE
      - WISEMCU_64KB_RAM_IN_USE
      - WISEMCU_128KB_RAM_IN_USE
      - WISEMCU_192KB_RAM_IN_USE
      - WISEMCU_256KB_RAM_IN_USE
      - WISEMCU_320KB_RAM_IN_USE

    ```C
    //!Configure RAM Usage and Retention Size
    sl_si91x_configure_ram_retention(WISEMCU_192KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);
    ```

3. Invoke the API to put the MCU to sleep.
    - To sleep with RAM retention:

      ```C
      //!Sleep With RAM Retention 
      sl_si91x_trigger_sleep(SLEEP_WITH_RETENTION, DISABLE_LF_MODE, WKP_RAM_USAGE_LOCATION, (uint32_t) RSI_PS_RestoreCpuContext, IVT_OFFSET_ADDR, RSI_WAKEUP_FROM_FLASH_MODE);
      ```

    - To sleep without RAM retention:

      ```C
      //!Sleep Without RAM Retention 
      sl_si91x_trigger_sleep(SLEEP_WITHOUT_RETENTION, DISABLE_LF_MODE, WKP_RAM_USAGE_LOCATION, (uint32_t) RSI_PS_RestoreCpuContext, IVT_OFFSET_ADDR, RSI_WAKEUP_FROM_FLASH_MODE);
      ```

    - The following are the parameters passed to the `sl_si91x_trigger_sleep()` API:

    | **Parameters** | **Values** |
    |-----------|---------------|
    |Sleep Type |  SLEEP_WITH_RETENTION / SLEEP_WITHOUT_RETENTION |
    |lf_clk_mode | DISABLE_LF_MODE |
    |stack_address | WKP_RAM_USAGE_LOCATION (0x24061EFC) |
    |jump_cb_address | (uint32_t) RSI_PS_RestoreCpuContext |
    |vector_offset| IVT_OFFSET_ADDR (0x8202000 - common flash) |
    |        | 0x8012000 (dual flash)|
    |        | 0xA001000 (PSRAM) |
    |mode    | RSI_WAKEUP_FROM_FLASH_MODE |

4. Invoke the following API function to check the wakeup status.

    ```C
    //!Get wakeup source 
    RSI_PS_GetWkpUpStatus();
    ```

    - The following table describes the wakeup status bits:

    | **NPSS interrupt number** | **Interrupt Number in VIT** | **NPSS Interrupt**|
    |-----------|---------------|--------------|
    |0  |  20  |  uulp_wdt_intr |
    |1-5|  21  | uulp_gpio_intr     |
    |6-9|  22  | uulp_cmp_intr  |
    |10  | 22  | uulp_sysrtc_intr |
    |11  | 23  | uulp_bod_intr |
    |12  | 24  |  uulp_button_intr |
    |13  |25  | uulp_sdc_intr    |
    |14  | 26 | uulp_wakeup_intr                |
    |15| 27  | uulp_alarm_intr            |
    |16| 28 | uulp_alarm_intr                    |
    |17| 29  | uulp_sec_intr            |
    |18| 29  |uulp_msec_intr              |

>**Note:**
>* rsi_deepsleep_soc.c file should compiled to SRAM.
>
>* Refer to the [Wi-Fi - TCP Tx on Periodic Wakeup (SoC)](https://github.com/SiliconLabs/wiseconnect/tree/master/examples/si91x_soc/wlan/tcp_tx_on_periodic_wakeup) example for a detailed >example of M4 sleep wakeup.
>
>* Enable SL_SI91X_ENABLE_LOWPWR_RET_LDO macro to optimize the deepsleep >power number, by default it is disabled.

### Front End Switch Selection GPIOs

 It is possible to configure the front-end switch GPIO pin selection through opermode.

The following table describes the font-end switch selection for the SiWx917.

|**BIT[30]** |**BIT[29]** | **ANT_SEL_0(VC3)** | **ANT_SEL_1(VC2)** | **ANT_SEL_2(VC1)**|
|---------|---------|--------|-------|------|
|0|0|Reserved|Reserved|Reserved|
|0|1|ULP GPIO 4|ULP GPIO 5|ULP GPIO 0|
|1|0|Internal Switch|Internal Switch|Internal Switch|
|1|1|Reserved|Reserved|Reserved|

> **Note:**
>* SiWx917 has an integrated on-chip transmit/receive (T/R) switch. This Internal RF Switch configuration uses internal logic present in the IC, and GPIOs are not needed. RF_BLE_TX (8dbm) mode is not supported in this configuration.
>* VC1, VC2, and VC3 are control voltage pins of the radio frequency (RF) switch. Please see the Reference Schematics for details.
### Preprocessor Macros

#### SLI_SI91X_MCU_ENABLE_RAM_BASED_EXECUTION

Enable this macro in order to execute selected WiSeConnect SDK files from RAM. This is required to achieve atomicity during the simulataneous access of the flash by both the MCU and NWP.

This is necessary in the following scenarios:
* Wireless de-initialization,
* Firmware update,
* Throughput, and
* Power save

The `SLI_SI91X_MCU_ENABLE_RAM_BASED_EXECUTION` macro will be enabled if the **device_needs_ram_execution** component is present in the example's slcp file. The slcp file can be edited in the example project in Simplicity Studio.

> **Note:** We recommend you install this component when your application implements one or more of the above scenarios.


### SLI_SI91X_MCU_4MB_LITE_IMAGE

The current wireless flash image is 1.6MB. An lite image has been implemented to free up the space for MCU applications.
Lite config is used for 4MB SoC OPN where 1.3 MB allocated for the TA image and ~1MB reserved for the M4 image

The `SLI_SI91X_MCU_4MB_LITE_IMAGE` macro will be enabled if the **lite_image_for_4mb** component is installed.

For low power M4 sleep states such as PS2, PS3, and PS4, certain files must be run from RAM memory.. Please refer [Power manager integration guide](
https://github.com/SiliconLabs/wiseconnect/blob/master/examples/si91x_soc/service/sl_si91x_power_manager_m4_wireless/resources/power_manager_integration_guide/power_manager_integration.pdf
) for more details

## Memory Organization

This section provides an overview of the different memory regions of the SiWx917.

The SiWx917 SoC contains the following memory components:
* On-chip SRAM of 192KB, 256KB, or 320KB depending on chip configuration
* 8KB memory in the ultra low power (ULP) peripheral subsystem, primarily used by the ULP MCU peripherals
* 64KB of ROM
* Quad SPI flash memory up to 8MB depending on package configuration
* Quad SPI PSRAM up to 8MB depending on package configuration
* e-Fuse of 32 bytes

Detailed information on the memory components above coming soon in the **SiWx917 SoC Memory Map Application Note**.

### PSRAM

The PSRAM on the SiWx917 provides additional RAM space to the MCU application beyond the SRAM. It is available in either the stacked or external form depending on package configuration. The stacked version offers better secure execution of the application and secure data storage. The MCU application can use the PSRAM for additional RAM space beyond the SRAM. 

The SiWx917 provides the PSRAM in a ready-to-use state upon boot up. During boot up, the bootloader:
- Initializes the PSRAM die according to the chip and PSRAM configurations in the master boot record (MBR).
- Makes the PSRAM available as in-system memory. 

The application may use the PSRAM for different memory segments like .data, .bss, .stack, .heap, .text, or user defined segments. Different PSRAM components are available in the Simplicity Studio application project depending on the memory segments required. See the components starting from **BSS Segment in PSRAM** in the [Peripherals](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-migr-sdk-changes/application-components#peripherals) section of the **Application Components** documentation page.

The PSRAM may be used for runtime data storage in addition to MCU application execution.

The PSRAM address space is mapped to 0x0A00_0000 - 0x0AFF_FFFF and 0x0B00_0000 - 0x0BFF_FFFF which is cached via D-cache and I-cache to improve the memory access performance for both data and instructions respectively. The address 0x0A00_0000 - 0x0AFF_FFFF drives the chip select CSN0 and 0x0B00_0000 - 0x0BFF_FFFF drives the chip select CSN1 of the PSRAM controller.

There is a provision to modify the PSRAM configuration settings in addition to boot up and default settings, with the help of driver APIs which are part of the WiSeConnect SDK. These additional configurations include interface mode, read/write type, clock, pinset, and others. The WiSeConnect SDK offers various [examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/) to demonstrate the use of PSRAM memory and driver APIs.

The PSRAM interface comes with security features which allow the user to protect the data stored in the PSRAM. It is possbile to configure up to 4 secure segments (start address and length) in the MBR. The inline AES engine in the Quad SPI controller encrypts and decrypts the data on the fly. It supports the CTR mode of encryption and decryption with a key size of 128 or 256 bits. The security configurations can only be written once during system boot up and cannot be modified later, which provides write protection to the security configuration.

The WiSeConnect SDK provides a power save mechanism for the PSRAM which effectively utilizes the hardware power handles and sleep features of the PSRAM die across sleep and wake up to minimize power consumption.

For more information on the memory architecture and hardware interfaces of PSRAM, see the **SiWx917 Memory Architecture** and **SiWx917 SPI Flash/PSRAM Controller** sections of the **SiWx917 Reference Manual** for details (contact [sales](https://www.silabs.com/about-us/contact-sales) for access).

> **Note:** Currently, the radio boards map the CSN0 to drive the PSRAM die and thus enable CSN0 address space 0x0A00_0000-0x0AFF_FFFF.

### Flash and PSRAM Combinations

This section describes the package combinations of Flash and PSRAM provided by the SiWx917. The combination may be selected by the user depending on the application being developed.

The available PSRAM options depend on the flash configuration of the SiWx917 package:
* In the common flash configuration, both the NWP and MCU will use the same flash memory which is either stacked or external.
* In the dual flash configuration, the NWP uses stacked flash and MCU uses external flash.

The following table shows the possible combinations and the available options to add on PSRAM:

| **Modes**    | **Flash type**         | **Flash Size** | **PSRAM (optional)**              
| -------------|------------------------|----------------|---------------------
| Common Flash | **Stacked**            | 4MB            | 2MB (external) or 8MB (external)  
|              | **External**           | 8MB            | 2MB (stacked)                     
|              |                        | 16MB           | 8MB                               
| Dual Flash   | **Stacked + External** | 4MB + 8MB      | 2MB                               
|              |                        | 4MB + 16MB     | 8MB                               

See the **OPN** section in the Datasheet for more information.

### NVM3

This section provides an overview of the NVM3 in SiWx917 SoC.

**NVM3:**

The NVM3 provides a means to write and read data objects (key/value pairs) stored in Flash. Wear-leveling is applied to reduce erase and write cycles and maximize flash lifetime. The driver is resilient to power loss and reset events, ensuring that objects retrieved from the driver are always in a valid state. A single NVM3 instance can be shared among several wireless stacks and application code, making it well-suited for multiprotocol applications.

For more detailed information about NVM3, refer to [Third Generation NonVolatile Memory (NVM3) Data Storage](https://www.silabs.com/documents/public/application-notes/an1135-using-third-generation-nonvolatile-memory.pdf).

**Working of NVM3 in Si91x Common Flash:**

![NVM3 implementation in Si91x common flash](./resources/NVM3_implementation_in_Si91x_Common_Flash.png)

**NVM3 installation for Si91x:**

NVM3 in Si91x can be installed in two methods,

Procedure 1:

Installing from Simplicity Studio,

1. After launching the application, double click on application slcp in Project Explorer

![application slcp](./resources/app_slcp.png)

2. Select SOFTWARE COMPONENTS in slcp

![software components](./resources/software_components.png)

3. Type "nvm3 for si91x" in search and select NVM3 for Si91x

![searching nvm3](./resources/slcp_search.png)

4. Click install

![install component](./resources/install_component.png)

Procedure 2:

Add the below lines in the application slcp and launch the project in Simplicity Studio,

requires:

  -name: nvm3_lib

**NVM3 region in flash:**

NVM3 region is allocated at the end of the M4 flash. A separate nvm3 section is used in the linker script with configurable size to allocate nvm3 memory.

![NVM3 memory allocation](./resources/NVM3_memory_allocation.png)

For example, refer to the below section of the .map file for nvm3 memory allocation in Si91x common flash (brd4338a), by default nvm3 size is 36 kB.

![NVM3 memory allocation example](./resources/NVM3_memory_allocation_example.png)

**Configuring NVM3 Size:**

1. Open nvm3_default_config.h from wiseconnect3\components\device\silabs\si91x\mcu\drivers\service\nvm3\inc\
2. Update NVM3_DEFAULT_NVM_SIZE

For example, After updating NVM3_DEFAULT_NVM_SIZE to 51200, the nvm3 region in .map is updated as,

![NVM3 memory configuration example](./resources/NVM3_memory_config_example.png)

**NVM3 usage in Si91x:**

Wireless initialization needs to be done before using NVM3  APIs in common flash as TA M4 communication is required to perform Flash writes/erases.

1. After successful wireless initialization nvm3_initDefault() API will open nvm3 instance with default parameters.
2. Any of the above default nvm3 parameters can be overridden by updating corresponding macros in nvm3_default_config.h
3. In NVM3 data is stored in the form of objects, an object is a combination of (key + data)
4. A key can be an identifier of the data stored in NVM3.
5. After successfully initializing NVM3, objects can be written in nvm3 using nvm3_writeData(),
6. Data can be read with nvm3_readData() using corresponding object key.
7. Objects can also be deleted using nvm3_deleteObject().

**Note:**

1. Wireless initialization is only required for Si91x common flash. For Si91x dual flash, nvm3_initDefault() can be directly called without wireless init
2. NVM3 APIs should not be called from an ISR
3. For more information about NVM3 usage, refer to [NVM3 - NVM Data Manager](https://docs.silabs.com/gecko-platform/3.1/driver/api/group-nvm3).

## Chip/Module Programming

### ISP Mode

The SiWx917 SoC provides a configuration option to enter the in-system programming (ISP) mode by asserting and resetting GPIO_34.

ISP mode enables the programming or re-programming of the flash memory using the security bootloader. It is possible to direct the security bootloader to boot up in ISP mode by pulling down the GPIO_34 pin with a 4.7K ohms resistor. The security bootloader does not initiate the execution of the code when ISP mode is enabled. Also, when the application code uses JTAG pins for functionality, entering ISP mode causes the JTAG to be disabled so that the flash may be re-programmed.

ISP mode is supported via following interfaces:

| **Interface** |         **Pins**          |
|---------------| --------------------------|
| **UART**      | RX: GPIO_8, TX: GPIO_9    |
| **SDIO**      | GPIO_25 - GPIO_30         |
| **HSPI**      | GPIO_25 - GPIO_30         |

## Trace and Debug Interfaces

Trace and Debug functions are integrated into the MCU's ARM Cortex M4 processor, whose TPIU supports two output modes:
* Clocked mode, using up to 4-bit parallel data output ports
* SWV mode, using single-bit SWV output

> **Note:** Trace and Debug components require a CLK supply on the **M4SS_TRACE_CLKIN pin**.

![Trace and Debug Interfaces](./resources/trace_debug_interface.png)

### Serial Wire Viewer (SWV) Support

The SWV provides real-time data trace information from various sources within a Cortex-M4 device. It is transmitted via the **SWO** pin while your system processor continues to run at full speed. Information is available from the ITM and DWT units. SWV data trace is available via the SWO pin. 

To setup the SWV, configure GPIO_12 pin in mode 8 (MCU_CLK_OUT) and connect to back to GPIO_15 pin configured in GPIO mode 6 (M4SS_TRACE_CLKIN). The MCU_CLK_OUT has programmable divider option from MCU clock. The MCU_CLK_OUT frequency must be less than 40Mhz to use the SWO function. After configuration, data traces can be observed with the supporting debug probes.

MCU_CLK_OUT register details are present in the **SiWx917 MCU HP Clock Architecture > MCUHP_CLK_CONFIG_REG3** section of the  **SiWx917 Reference Manual** for details (contact [sales](https://www.silabs.com/about-us/contact-sales) for access).

### Embedded Trace Macrocell (ETM) Support

The ETM provides high bandwidth instruction traces via four dedicated trace pins accessible on the 20-pin Cortex debug + ETM connector. The MCU_CLK_OUT frequency must be in the range of 40 Mhz to 90 MHz to trace instructions using ETM.

To setup ETM, configure the trace pins (GPIO_52 to GPIO_57) in mode 6 and GPIO_12 pin in mode 8 (MCU_CLK_OUT). By default, on power up these GPIO pins are mapped to the Cortex debug + ETM connector (20-pin). After configuration, instruction traces can be observed with the supporting debug probes.

> **Note:** A free running clock must be present on MCU_CLK_OUT (GPIO_12) before programming the trace pins otherwise the IDE will display the **Trace HW not present** error.

### VCOM

The virtual COM (VCOM) port is available on the wireless pro kit mainboard (BRD4002A) and supports the following features:
* Flash, erase and debug over SWD
  * If the ULP UART peripheral is configured, VCOM cannot be used for debug prints because the ULP UART is mapped to the VCOM.
  * To enable both the ULP UART and VCOM, configure UART1/UART2 instance for debug prints. In this case, a USB-to-TTL converter is required. See the [Console Input](https://docs.silabs.com/wiseconnect/3.1.0/wiseconnect-getting-started/getting-started-with-soc-mode#enter-the-console-input-for-other-boards) section for the BRD4325 series boards for instructions.
  * If the ULP UART is functioning in the PS2 state, the UART1/UART2 cannot be used for debug prints
* UART RX and TX, for use in command line applications and debug prints
  * In this mode, ULP_GPIO_9 (RX) and ULP_GPIO_11 (TX) will be mapped to the MCU UART
* Flash programming in ISP mode
  * In this mode, GPIO_8 (RX) and GPIO_9 (TX) will be mapped to TA UART

> **Note:** Debug logs from the NWP can be fetched via the NWP UART which is available on the EXP header's EXP14 (UART_RX) and EXP12 (UART_TX) pins.

## Firmware Upgrade
Firmware  is  the  software  that  is  embedded  into  a  hardware  device.  It  contains  a  set  of  commands  that  control  the behavior  of  a  network  device. Whenever  a  new  firmware  version  is  available,  it  is  recommended  that  users  update their  devices  to  the  latest  version.  Complete  details  about  the  latest  firmware  will  be  available  in  the  Release  Notes (shared  as  part  of  the  release  package),  which  will  help  the  users  decide  whether  to  update  to  the  new  firmware  or not.

### Firmware File Format

The TA Bootloader uses a proprietary format for its upgrade images, called RPS. These files have extension “.rps”. 

The RPS Format is a binary executable format understood by the Bootloader to conduct integrity and authenticity verification, as well as to load and execute the application. The Firmware Image in RPS format includes an RPS header, Boot descriptors, Application's binary image and an optional trailer (digital signature).

### SiWx917 Firmware Load and Update Process

The firmware load process  loads  firmware  onto  SiWx917  device  for  the  first  time  in  the  case  of  new  devices. The Firmware update process updates SiWx917 devices with the latest firmware by replacing the firmware already existing in the device. 

The steps are as follows: 
1. To  update  existing  firmware  or  a  device  without  firmware,  download  the  new  firmware  file  to  the  device’s  flash memory from host (MCU/PC) through any host interface or through OTA process.
2. After reboot, the current firmware is replaced by the new firmware in flash memory, and the device is updated with the new firmware.

### SiWx917 Firmware Update Mechanisms

Firmware in the SiWx917 device can be updated using the following mechanisms:

1. Firmware update via Over-The-Air (OTA): In this mechanism firmware in the device can be updated by the following methods.
   * HTTP/S: Firmware is updated by downloading the firmware file from a remote HTTP/S or cloud server via Wi-Fi. The firmware file is directly downloaded to TA flash location.
   * M4  as  Host:  Using  host  interfaces  –  SPI/UART/SDIO  or  a  remote  TCP  server  via  Wi-Fi  or  BLE,  the firmware  is  reaching  in  chunks  to  M4.  The  user  can  choose  to  send  the  firmware  to  TA  Bootloader  for upgrade or save in the external flash as per their requirements.

2. Firmware update via Bootloader: In this mechanism firmware in the device can be updated by the following methods.
   * Kermit:  Firmware  is  updated  using  Kermit  protocol  in  a  serial  terminal  like  Tera  Term  running  in  a Windows/Linux PC The connected to the device through UART interface in ISP mode.
   * Simplicity Commander Tool/ Command Line Interface (CLI): Using the Simplicity Commander tool or by using the CLI commands the firmware is updated.

> **Note:** SiWx917  also  have  Secure  and  Non-Secure  Firmware  update.  The  above  mechanisms  are  same  for  both secure and non-secure except that the firmware does security related integrity checks before loading the device with the new firmware.

### Secure Firmware Update

In  case  of  bootloader  and  OTA,  secure  firmware  update  can be carried out by  enabling   the  security  in  the  firmware  image using  the  SiWx917  Manufacturing  Utility  (Refer  to  the  SiWx917  Manufacturing  Utility  User  Guide).  The  process  of firmware  update  remains  same  for  Non-Secure  and  Secure  Firmware  update  except  that  in  the  case  of  secure firmware image the integrity checks are done.

> **Note:** For more information on the Firmware upgradation, see the Firmware Update App Note.
