#pragma once

#include "em_device.h"
#include "sl_clock.h"

typedef USART_TypeDef   sl_usart_peripheral_t;
//typedef I2C_TypeDef     sl_i2c_registers_t;
typedef GPIO_PORT_TypeDef  sl_gpio_peripheral_t;

typedef LDMA_TypeDef    sl_dma_peripheral_t;
typedef LDMA_CH_TypeDef sl_dma_channel_register_t;

typedef enum
{
    SL_USART_0,
    SL_USART_1,
    SL_USART_2,
} sl_usart_id_t;

typedef enum
{
    SL_SPI_0,
    SL_SPI_1,
    SL_SPI_2,
} sl_spi_id_t;

#define   sl_usart_0    (*(sl_usart_peripheral_t*)(0x40058000UL))  /**< USART0 base address  */
#define   sl_usart_1    (*(sl_usart_peripheral_t*)(0x4005C000UL))  /**< USART1 base address  */
#define   sl_usart_2    (*(sl_usart_peripheral_t*)(0x40060000UL))  /**< USART2 base address  */

extern sl_usart_peripheral_t* const sl_usart_peripherals[3];

extern const sl_clock_id_t sl_usart_clocks[3];

