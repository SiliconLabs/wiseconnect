#include "sl_device_registers.h"

//sl_usart_peripheral_t sl_usart_0 LOCATE(0x40010000UL);  /**< USART0 base address  */
//sl_usart_peripheral_t sl_usart_1 LOCATE(0x40010400UL);  /**< USART1 base address  */
//sl_usart_peripheral_t sl_usart_2 LOCATE(0x40010800UL);  /**< USART2 base address  */
//sl_usart_peripheral_t sl_usart_3 LOCATE(0x40010C00UL);  /**< USART3 base address  */
//sl_usart_peripheral_t sl_usart_4 LOCATE(0x40011000UL);  /**< USART4 base address  */
//sl_usart_peripheral_t sl_usart_5 LOCATE(0x40011400UL);  /**< USART5 base address  */


sl_usart_peripheral_t* const sl_usart_peripherals[] =
{
   [SL_USART_0] = &sl_usart_0,
   [SL_USART_1] = &sl_usart_1,
   [SL_USART_2] = &sl_usart_2,
   [SL_USART_3] = &sl_usart_3,
   [SL_USART_4] = &sl_usart_4,
   [SL_USART_5] = &sl_usart_5,
};


const sl_clock_id_t sl_usart_clocks[] =
{
#ifdef SL_CLOCK_USART0
   [SL_USART_0] =  SL_CLOCK_USART0,
#endif
#ifdef SL_CLOCK_USART1
   [SL_USART_1] =  SL_CLOCK_USART1,
#endif
#ifdef SL_CLOCK_USART2
   [SL_USART_2] =  SL_CLOCK_USART2,
#endif
#ifdef SL_CLOCK_USART3
   [SL_USART_3] =  SL_CLOCK_USART3,
#endif
#ifdef SL_CLOCK_USART4
   [SL_USART_4] =  SL_CLOCK_USART4,
#endif
#ifdef SL_CLOCK_USART5
   [SL_USART_5] =  SL_CLOCK_USART5,
#endif
};
