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
   rom   (rx)  : ORIGIN = 0x8212000, LENGTH = 0x6e000
   ram   (rwx) : ORIGIN = 0x400, LENGTH = 0x2fc00
 }
 MEMORY
 {
   udma0   (rwx)  : ORIGIN = 0x2fc00, LENGTH = 0x400
   udma1   (rwx)  : ORIGIN = 0x24060000, LENGTH = 0x400
 }

ENTRY(Reset_Handler)
 
SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
		
		KEEP(*(.reset_handler))
		*(EXCLUDE_FILE( *rsi_common.c.o *rsi_driver.c.o *rsi_driver_event_handlers.c.o *rsi_events.c.o *rsi_os_none.c.o *rsi_pkt_mgmt.c.o *rsi_queue.c.o *rsi_scheduler.c.o *rsi_timer.c.o *rsi_utils.c.o *rsi_wlan.c.o *rsi_common_apis.c.o *rsi_wlan_apis.c.o *rsi_apis_non_rom.c.o *rsi_hal_mcu_timer.c.o *rsi_hal_mcu_interrupt.c.o *rsi_hal_mcu_m4.c.o *rsi_hal_mcu_platform_init.c.o *rsi_apis_rom.c.o *rsi_events_rom.c.o *rsi_hal_mcu_m4_rom.c.o *rsi_pkt_mgmt_rom.c.o *rsi_queue_rom.c.o *rsi_scheduler_rom.c.o *rsi_socket_rom.c.o *rsi_utils_rom.c.o *rsi_nwk_rom.c.o *rsi_os_wrapper.c.o *port.c.o *heap_4.c.o *timers.c.o *croutine.c.o *event_groups.c.o *list.c.o *queue.c.o *tasks.c.o *stream_buffer.c.o *network_sapi_wrapper.c.o *rsi_deepsleep_commonflash.c.o ) .text*)
		
		
		/* .ctors */
		*crtbegin.c.o(.ctors)
		*crtbegin?.c.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.c.o *crtend.c.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)
		
		/* .dtors */
		*crtbegin.c.o(.dtors)
		*crtbegin?.c.o(.dtors)
		*(EXCLUDE_FILE(*crtend?.c.o *crtend.c.o) .dtors)
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
	
	/* _sidata is used in coide startup code */
	_sidata = __etext;
	
	.data :
	{
		__data_start__ = .;
		
		/* _sdata is used in coide startup code */
		_sdata = __data_start__;
		
		KEEP(*(.ramVector))
		KEEP(*(.init))
		KEEP(*(.fini))
		*(.rodata*)
		
		*(vtable)
		*(.data*)
		
		*rsi_common.c.o(.text*)
		*rsi_driver.c.o(.text*)
		*rsi_driver_event_handlers.c.o(.text*)
		*rsi_events.c.o(.text*)
		*rsi_os_none.c.o(.text*)
		*rsi_pkt_mgmt.c.o(.text*)
		*rsi_queue.c.o(.text*)
		*rsi_scheduler.c.o(.text*)
		*rsi_timer.c.o(.text*)
		*rsi_utils.c.o(.text*)
		*rsi_wlan.c.o(.text*)
		*rsi_common_apis.c.o(.text*) 
		*rsi_wlan_apis.c.o(.text*)
		*rsi_apis_non_rom.c.o(.text*)
		*rsi_hal_mcu_timer.c.o(.text*)
		*rsi_hal_mcu_m4.c.o(.text*)
		*rsi_hal_mcu_interrupt.c.o(.text*)
		*rsi_hal_mcu_platform_init.c.o(.text*)
		*rsi_hal_mcu_m4_rom.c.o(.text*)
		*rsi_apis_rom.c.o(.text*)
		*rsi_events_rom.c.o(.text*)
		*rsi_pkt_mgmt_rom.c.o(.text*)
		*rsi_queue_rom.c.o(.text*)
		*rsi_scheduler_rom.c.o(.text*)
		*rsi_socket_rom.c.o(.text*)
		*rsi_utils_rom.c.o(.text*)
		*rsi_nwk_rom.c.o(.text*)
		*rsi_os_wrapper.c.o(.text*)
		*timers.c.o(.text*)
		*croutine.c.o(.text*)
		*event_groups.c.o(.text*)
		*list.c.o(.text*)
		*queue.c.o(.text*)
		*stream_buffer.c.o(.text*)
		*tasks.c.o(.text*)
		*heap_4.c.o(.text*)
		*port.c.o(.text*)
		*network_sapi_wrapper.c.o(.text*)
        *rsi_deepsleep_commonflash.c.o(.text*)
		
		
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
		
		/* _edata is used in coide startup code */
		_edata = __data_end__;
	} > ram AT> rom 
	
	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		_sbss = __bss_start__;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
		_ebss = __bss_end__;
	} > ram 
		
	.heap (COPY):
	{
		__end__ = .;
		_end = __end__;
		end = __end__;
		*(.heap*)
		__HeapLimit = .;
	} > ram 
	
	/* .stack_dummy section doesn't contains any symbols. It is only
	* used for linker to calculate size of stack sections, and assign
	* values to stack symbols later */
	.co_stack (NOLOAD):
	{
		. = ALIGN(8);
		*(.co_stack .co_stack.*)
	} > ram 
	.udma_addr0 :
	{
		*(.udma_addr0*)
	} > udma0 AT> rom 

	.udma_addr1 :
	{
		*(.udma_addr1*)
	} > udma1 AT> rom	
			
	/* Set stack top to end of ram , and stack limit move down by
	* size of stack_dummy section */
	__StackTop = ORIGIN(ram ) + LENGTH(ram );
	__StackLimit = __StackTop - SIZEOF(.co_stack);
	PROVIDE(__stack = __StackTop);
	
	/* Check if data + heap + stack exceeds ram  limit */
	ASSERT(__StackLimit >= __HeapLimit, "region ram  overflowed with stack")
}
