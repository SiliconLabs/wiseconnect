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
    
		  *(EXCLUDE_FILE(*sl_si91x_dma* *sl_core_cortexm* *sl_platform_wireless* *sl_si91x_power_manager_wakeup_handler* *sl_wifi* *sl_si91x_m4_ps* *rsi_sysrtc* *sl_si91x_clock_manager* *sli_si91x_clock_manager* *sl_sleeptimer_hal_si91x_sysrtc* *em_core* *sl_sleeptimer* *sl_si91x_low_power_tickless_mode* *sl_si91x_power_manager* *sli_si91x_power_manager* *sl_si91x_power_manager_handler* *sl_si91x_power_manager_debug*  *cmsis_gcc* *sbrk* *libm_a* *libc_a* *_arm_* *libm_a-s_ceil* *snprintf* *sensorhub_aws_app* *rsi_hal_mcu_m4_rom* *memcpy* *sensorhub*  *sensorhub_app* *sl_si91x_bus* *sl_si91x_driver* *sli_wifi_command_engine* *sli_si91x_wifi_event_handler* *rsi_hal_mcu_m4_ram* *croutine* *event_groups* *list* *stream_buffer* *tasks* *timers* *freertos_umm_malloc_host* *malloc_buffers* *sl_rsi_utility* *port* *heap_* *rsi_ps_ram_func* *I2C* *SPI* *UDMA* *aux_reference_volt_config* *rsi_adc* *rsi_crc* *rsi_dac* *rsi_gpdma* *rsi_i2c* *rsi_qspi* *rsi_spi* *rsi_udma* *rsi_udma_wrapper* *sensor_hub* *rsi_bod* *rsi_temp_sensor* *rsi_wwdt* *sl_si91x_adc* *sl_si91x_ssi* *hub_hal_intf* *accelerometerSensor_hal* *adxl345* *adc_sensor_driver* *adc_sensor_hal* *apds9960* *apds9960Sensor_hal* *bh1750fvi* *lightsensor_hal* *lm75* *tempSensor_hal* *cmsis_os2* *port* *queue* *sl_rsi_utility* *tasks* *clock_update* *rsi_deepsleep_soc* *rsi_egpio* *rsi_ipmu* *ipmu_apis* *rsi_pll* *rsi_power_save* *rsi_system_config* *rsi_time_period* *rsi_ulpss_clk* *system_si91x* *sl_si91x_power_manager* *sli_si91x_power_manager* *power_manager_m4_wireless_example* *sl_si91x_button* *sl_si91x_button_instances* *app* *main* *stdio* *puts* *wbuf* *writer* *memchr* *memset* *nano-vfprintf* *nano-vfprintf_i* *printf* *lock* *fflush* *rsi_debug* *rsi_usart* *syscalls* *USART* *sl_si91x_calendar* *rsi_rtc* *sl_si91x_ulp_timer* *rsi_timers* *sl_si91x_peripheral_gpio* *sl_si91x_sdc* *sl_si91x_driver_gpio* ) .text*)


		/* .ctors */
		*crtbegin*(.ctors)
		*crtbegin?*(.ctors)
		*(EXCLUDE_FILE(*crtend?* *crtend*) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)
		
		/* .dtors */
		*crtbegin*(.dtors)
		*crtbegin?*(.dtors)
		*(EXCLUDE_FILE(*crtend?* *crtend*) .dtors)
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
		*cmsis_gcc*(.text*)
		*_arm_*(.text*)
		*libc_a*(.text*)
		*libm_a*(.text*)
		*sbrk*(.text*)
		*libm_a-s_ceil*(.text*)
		*cmsis_os2*(.text*)
		*port*(.text*)
		*queue*(.text*)
		*sl_rsi_utility*(.text*)
		*tasks*(.text*)
		*clock_update*(.text*)
		*rsi_deepsleep_soc*(.text*)
		*rsi_egpio*(.text*)
		*rsi_ipmu*(.text*)
		*ipmu_apis*(.text*)
		*rsi_pll*(.text*)
		*rsi_power_save*(.text*)
		*rsi_system_config*(.text*)
		*rsi_time_period*(.text*)
		*rsi_ulpss_clk*(.text*)
	    *sl_si91x_clock_manager*(.text*)
		*sli_si91x_clock_manager*(.text*)		
		*system_si91x*(.text*)
		*app*(.text*)
		*main*(.text*)
		*stdio*(.text*)
		*puts*(.text*)
		*wbuf*(.text*)
		*writer*(.text*)
		*memchr*(.text*)
		*memset*(.text*)
		*memcpy*(.text*)
		*nano-vfprintf*(.text*)
		*nano-vfprintf_i*(.text*)
		*printf*(.text*)
		*lock*(.text*)
		*fflush*(.text*)
		*rsi_debug*(.text*)
		*rsi_usart*(.text*)
		*syscalls*(.text*)
		*USART*(.text*)
		*sl_si91x_calendar*(.text*)
		*rsi_rtc*(.text*)
		*sl_si91x_ulp_timer*(.text*)
		*rsi_timers*(.text*)
		*sl_si91x_peripheral_gpio*(.text*)
		*sl_si91x_driver_gpio*(.text*)
		*rsi_ps_ram_func*(.text*)
		*I2C*(.text*)
		*SPI*(.text*)
		*aux_reference_volt_config*(.text*)
		*rsi_adc*(.text*)
		*rsi_crc*(.text*)
		*rsi_dac*(.text*)
		*rsi_gpdma*(.text*)
		*rsi_i2c*(.text*)
		*rsi_qspi*(.text*)
		*rsi_spi*(.text*)
		*rsi_udma*(.text*)
		*rsi_udma_wrapper*(.text*)
		*sensor_hub*(.text*)
		*rsi_bod*(.text*)
		*rsi_temp_sensor*(.text*)
		*rsi_wwdt*(.text*)
		*sl_si91x_adc*(.text*)
		*sl_si91x_ssi*(.text*)
		*hub_hal_intf*(.text*)
		*accelerometerSensor_hal*(.text*)
		*adxl345*(.text*)
		*adc_sensor_driver*(.text*)
		*adc_sensor_hal*(.text*)
		*apds9960*(.text*)
		*apds9960Sensor_hal*(.text*)
		*bh1750fvi*(.text*)
		*lightsensor_hal*(.text*)
		*lm75*(.text*)
		*tempSensor_hal*(.text*)
		*rsi_hal_mcu_m4_ram*(.text*)
		*rsi_hal_mcu_m4_rom*(.text*)
		*sl_si91x_driver*(.text*)
		*sl_si91x_bus*(.text*)
		*sli_wifi_command_engine*(.text*)
		*sli_si91x_wifi_event_handler*(.text*)
		*croutine*(.text*)
		*event_groups*(.text*)
		*list*(.text*)
		*stream_buffer*(.text*)
		*timers*(.text*)
		*freertos_umm_malloc_host*(.text*)
		*malloc_buffers*(.text*)
		*sl_rsi_utility*(.text*)
		*heap_*(.text*)
		*sensorhub_app*(.text*)
		*sensor_hub*(.text*)
		*sensorhub_aws_app*(.text*)
		*sl_si91x_sdc*(.text*)
		*snprintf*(.text*)
        *sl_si91x_power_manager*(.text*)
		*sli_si91x_power_manager*(.text*)
		*sl_si91x_power_manager_handler*(.text*)
		*sl_si91x_power_manager_debug*(.text*)
		*sl_si91x_low_power_tickless_mode*(.text*)
		*sl_si91x_power_manager_wakeup_handler*(.text*)
		*sl_platform_wireless*(.text*)
		*UDMA*(.text*)
		*sl_sleeptimer*(.text*)
		*em_core*(.text*)
		*sl_sleeptimer_hal_si91x_sysrtc*(.text*)
		*rsi_sysrtc*(.text*)
		*sl_si91x_m4_ps*(.text*)
		*sl_wifi*(.text*)
		*sl_core_cortexm*(.text*)
		*sl_si91x_dma*(.text*)
		
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
