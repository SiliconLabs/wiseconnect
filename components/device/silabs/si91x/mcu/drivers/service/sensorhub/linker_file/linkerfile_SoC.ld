/***************************************************************************//**
 * GCC Linker script for Silicon Labs devices
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/


 MEMORY
 {
   rom   (rx)  : ORIGIN = 0x8202000, LENGTH = 0x1fe000
   ram   (rwx) : ORIGIN = 0x400, LENGTH = 0x37000
 }
 MEMORY
 {
   udma0   (rwx)  : ORIGIN = 0x37400, LENGTH = 0x400
   udma1   (rwx)  : ORIGIN = 0x24061c00, LENGTH = 0x400
 }

ENTRY(Reset_Handler)
 
SECTIONS
{	

	.text :
	{
		KEEP(*(.isr_vector))
    
		  *(EXCLUDE_FILE(*sl_si91x_dma.o *sl_core_cortexm.o *sl_platform_wireless.o *sl_si91x_power_manager_wakeup_handler.o *sl_wifi.o *sl_si91x_m4_ps.o *rsi_sysrtc.o *sl_sleeptimer_hal_si91x_sysrtc.o *em_core.o *sl_sleeptimer.o *sl_si91x_low_power_tickless_mode.o *sl_si91x_power_manager.o *sli_si91x_power_manager.o *sl_si91x_power_manager_handler.o *sl_si91x_power_manager_debug.o  *cmsis_gcc.o *sbrk*.o *libm_a*.o *libc_a*.o *_arm_*.o *libm_a-s_ceil.o *snprintf*.o *sensorhub_aws_app.o *rsi_hal_mcu_m4_rom.o *memcpy*.o *sensorhub.o  *sensorhub_app.o *sl_si91x_bus.o *sl_si91x_driver.o *sli_si91x_multithreaded.o *rsi_hal_mcu_m4_ram.o *croutine.o *event_groups.o *list.o *stream_buffer.o *tasks.o *timers.o *freertos_umm_malloc_host.o *malloc_buffers.o *sl_rsi_utility.o *port.o *heap_*.o *rsi_ps_ram_func.o *I2C.o *SPI.o *UDMA.o *aux_reference_volt_config.o *rsi_adc.o *rsi_crc.o *rsi_dac.o *rsi_gpdma.o *rsi_i2c.o *rsi_qspi.o *rsi_spi.o *rsi_udma.o *rsi_udma_wrapper.o *sensor_hub.o *rsi_bod.o *rsi_temp_sensor.o *rsi_wwdt *sl_si91x_adc.o *sl_si91x_ssi.o *hub_hal_intf.o *accelerometerSensor_hal.o *adxl345.o *adc_sensor_driver.o *adc_sensor_hal.o *apds9960.o *apds9960Sensor_hal.o *bh1750fvi.o *lightsensor_hal.o *lm75.o *tempSensor_hal.o *cmsis_os2.o *port.o *queue.o *sl_rsi_utility.o *tasks.o *clock_update.o *rsi_deepsleep_soc.o *rsi_egpio.o *rsi_ipmu.o *ipmu_apis.o *rsi_pll.o *rsi_power_save.o *rsi_system_config.o *rsi_time_period.o *rsi_ulpss_clk.o *system_si91x.o *sl_si91x_power_manager.o *sli_si91x_power_manager.o *power_manager_m4_wireless_example.o *sl_si91x_button.o *sl_si91x_button_instances.o *app.o *main.o *stdio.o *puts.o *wbuf.o *writer.o *memchr.o *memset.o *nano-vfprintf.o *nano-vfprintf_i.o *printf.o *lock.o *fflush.o *rsi_debug.o *rsi_usart.o *syscalls.o *USART.o *sl_si91x_calendar.o *rsi_rtc.o *sl_si91x_ulp_timer.o *rsi_timers.o *sl_si91x_peripheral_gpio.o *sl_si91x_sdc.o *sl_si91x_driver_gpio.o ) .text*)


		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)
		
		/* .dtors */
		*crtbegin.o(.dtors)
		*crtbegin?.o(.dtors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
		*(SORT(.dtors.*))
		*(.dtors)
		
		KEEP(*(.eh_fram e*))			
	} > rom 
		
	
	.ARM.extab : 
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)			
	} > rom 
	
	
	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)			
	} > rom 
	 
	__exidx_end = .;
	__etext = .;
	
	


	
	/* _sidata is used in code startup code */
	_sidata = __etext;
	
	.data :
	
	{
		__data_start__ = .;
		
		/* _sdata is used in startup code */
		_sdata = __data_start__;
		KEEP(*(.ramVector))
		*(.data*)
		*(.rodata*)
		KEEP(*(.init))
		KEEP(*(.fini))
		*cmsis_gcc.o(.text*)
		*_arm_*.o(.text*)
		*libc_a*.o(.text*)
		*libm_a*.o(.text*)
		*sbrk*.o(.text*)
		*libm_a-s_ceil.o(.text*)
		*cmsis_os2.o(.text*)
		*port.o(.text*)
		*queue.o(.text*)
		*sl_rsi_utility.o(.text*)
		*tasks.o(.text*)
		*clock_update.o(.text*)
		*rsi_deepsleep_soc.o(.text*)
		*rsi_egpio.o(.text*)
		*rsi_ipmu.o(.text*)
		*ipmu_apis.o(.text*)
		*rsi_pll.o(.text*)
		*rsi_power_save.o(.text*)
		*rsi_system_config.o(.text*)
		*rsi_time_period.o(.text*)
		*rsi_ulpss_clk.o(.text*)
		*system_si91x.o(.text*)
		*app.o(.text*)
		*main.o(.text*)
		*stdio.o(.text*)
		*puts.o(.text*)
		*wbuf.o(.text*)
		*writer.o(.text*)
		*memchr.o(.text*)
		*memset.o(.text*)
		*memcpy*.o(.text*)
		*nano-vfprintf.o(.text*)
		*nano-vfprintf_i.o(.text*)
		*printf.o(.text*)
		*lock.o(.text*)
		*fflush.o(.text*)
		*rsi_debug.o(.text*)
		*rsi_usart.o(.text*)
		*syscalls.o(.text*)
		*USART.o(.text*)
		*sl_si91x_calendar.o(.text*)
		*rsi_rtc.o(.text*)
		*sl_si91x_ulp_timer.o(.text*)
		*rsi_timers.o(.text*)
		*sl_si91x_peripheral_gpio.o(.text*)
		*sl_si91x_driver_gpio.o(.text*)
		*rsi_ps_ram_func.o(.text*)
		*I2C.o(.text*)
		*SPI.o(.text*)
		*aux_reference_volt_config.o(.text*)
		*rsi_adc.o(.text*)
		*rsi_crc.o(.text*)
		*rsi_dac.o(.text*)
		*rsi_gpdma.o(.text*)
		*rsi_i2c.o(.text*)
		*rsi_qspi.o(.text*)
		*rsi_spi.o(.text*)
		*rsi_udma.o(.text*)
		*rsi_udma_wrapper.o(.text*)
		*sensor_hub.o(.text*)
		*rsi_bod.o(.text*)
		*rsi_temp_sensor.o(.text*)
		*rsi_wwdt.o(.text*)
		*sl_si91x_adc.o(.text*)
		*sl_si91x_ssi.o(.text*)
		*hub_hal_intf.o(.text*)
		*accelerometerSensor_hal.o(.text*)
		*adxl345.o(.text*)
		*adc_sensor_driver.o(.text*)
		*adc_sensor_hal.o(.text*)
		*apds9960.o(.text*)
		*apds9960Sensor_hal.o(.text*)
		*bh1750fvi.o(.text*)
		*lightsensor_hal.o(.text*)
		*lm75.o(.text*)
		*tempSensor_hal.o(.text*)
		*rsi_hal_mcu_m4_ram.o(.text*)
		*rsi_hal_mcu_m4_rom.o(.text*)
		*sl_si91x_driver.o(.text*)
		*sl_si91x_bus.o(.text*)
		*sli_si91x_multithreaded.o(.text*)
		*croutine.o(.text*)
		*event_groups.o(.text*)
		*list.o(.text*)
		*stream_buffer.o(.text*)
		*timers.o(.text*)
		*freertos_umm_malloc_host.o(.text*)
		*malloc_buffers.o(.text*)
		*sl_rsi_utility.o(.text*)
		*heap_*.o(.text*)
		*sensorhub_app.o(.text*)
		*sensor_hub.o(.text*)
		*sensorhub_aws_app.o(.text*)
		*sl_si91x_sdc.o(.text*)
		*snprintf*.o(.text*)
        *sl_si91x_power_manager.o(.text*)
		*sli_si91x_power_manager.o(.text*)
		*sl_si91x_power_manager_handler.o(.text*)
		*sl_si91x_power_manager_debug.o(.text*)
		*sl_si91x_low_power_tickless_mode.o(.text*)
		*sl_si91x_power_manager_wakeup_handler.o(.text*)
		*sl_platform_wireless.o(.text*)
		*UDMA.o(.text*)
		*sl_sleeptimer.o(.text*)
		*em_core.o(.text*)
		*sl_sleeptimer_hal_si91x_sysrtc.o(.text*)
		*rsi_sysrtc.o(.text*)
		*sl_si91x_m4_ps.o(.text*)
		*sl_wifi.o(.text*)
		*sl_core_cortexm.o(.text*)
		*sl_si91x_dma.o(.text*)
		
 		/* ipmu calibration data */
		*(.common_ipmu_ram*)

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);
		
		. = ALIGN(4);
		/* init data */
		
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);
		
		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);
		
		KEEP(*(.jcr*))
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;
		
		/* _edata is used in startup code */
		_edata = __data_end__; 
	} > ram AT> rom
		
	
	.bss (NOLOAD) :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .; 
	} > ram 
	

	/* Modify the below value to change the stack size */
	StackSize = 0x3000;
	.stack (NOLOAD):
	{
		__StackLimit = .;
		KEEP(*(.stack*))
		. = ALIGN(4);
		. += StackSize;
		__StackTop = .;
		PROVIDE(__stack = __StackTop);			
	} > ram
			
  	.heap (COPY):
  	{
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		_end = __end__;
		KEEP(*(.heap*))	
		. = ORIGIN(ram) + LENGTH(ram);
  		
		__HeapLimit = .;			
  	} > ram
  
	__heap_size = __HeapLimit - __HeapBase;
	.udma_addr0 :
	{
		*(.udma_addr0*)
	} > udma0 AT> rom
	

	.udma_addr1 :
	{
		*(.udma_addr1*)		
	} > udma1 AT> rom   
}
