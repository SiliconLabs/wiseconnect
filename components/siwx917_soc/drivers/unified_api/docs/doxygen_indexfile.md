@mainpage Summary

# Introduction
SL peripheral API for SI917 SOC

##API Documentation
| List of Modules |  Description |
|---------------| ------------------|
| [CALENDAR](@ref CALENDAR) | Calendar RTC can be used to set or get the current RTC time and date |
| [CONFIG-TIMER](@ref CONFIG-TIMER) | Configurable timers are used for counting clocks and events, capturing events on the GPIOs in input mode and outputting modulated signals |
| [DMA](@ref DMA) | DMA is an AHB based DMA Controller core that transfers data from a source peripheral or memory to a destination peripheral or memory over one or more AHB buses |
| [SLEEPTIMER](@ref SLEEPTIMER) | The Sleeptimer driver provides software timers, delays, timekeeping and date functionalities using a low-frequency real-time clock peripheral |
| [IOSTREAM](@ref IOSTREAM) | IO Stream is a platform module software that provides Input/Output functionalities by creating streams |
| [GPIO](@ref GPIO) | GPIO peripheral can support set, clear, toggle, programmed as input/output, generates pin and group interrupts etc. |
| [GSPI](@ref GSPI) | GSPI provides an I/O interface to a wide variety of SPI compatible peripheral devices |
| [I2C](@ref I2C) | There are four I2C Master/Slave controllers - two in the MCU HP peripherals (I2C1, I2C2), one in the NWP/security subsystem and one in the MCU ULP subsystem (ULP_I2C) |
| [SIO](@ref SIO) | Serial GPIO Peripheral supports regular GPIO and enhanced serial stream processing features for 16 GPIO pins |
| [SSI](@ref SSI) | There are three Synchronous Serial Interface (SSI) - one in the MCU HP peripherals (Master) and one in the MCU ULP subsystem (ULP Master) and one is MCU HP peripheral (Slave) |
| [ULP-TIMER](@ref ULP-TIMER) | ULP Timer is used in counting clocks, micro seconds, milli seconds, seconds and minutes with both ref clock and system (SoC) clock |
| [USART](@ref USART) | Universal Synchronous/Asynchronous Receiver/Transmitter Peripheral API |
| [WATCHDOG-TIMER](@ref WATCHDOG-TIMER) | WatchDog-timer is used generate an interrupt on timeout and a system-reset in case of system failure which can be caused by an external event like ESD pulse or due to a software failure |