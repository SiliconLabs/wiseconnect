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

ROM_START_ADDRESS = DEFINED(SI91X_SOC_ROM_START_ADDRESS) ? SI91X_SOC_ROM_START_ADDRESS : 0x8212000;
RAM_START_ADDRESS = DEFINED(SI91X_SOC_RAM_START_ADDRESS) ? SI91X_SOC_RAM_START_ADDRESS : 0xC;
STACK_SIZE       = 0x3000;
CO_STACK_SIZE    = 0x3000;

MEMORY
{
   mcu_rom (rx)  : ORIGIN = ROM_START_ADDRESS, LENGTH = 0x6e000
   mcu_ram (rwx) : ORIGIN = 0x0000000, LENGTH = 0x2fc00
}
 
MEMORY
{
   stack    (rwx) : ORIGIN = LENGTH(mcu_ram) - STACK_SIZE,       LENGTH = STACK_SIZE
   co_stack (rwx) : ORIGIN = ORIGIN(stack) - CO_STACK_SIZE,      LENGTH = CO_STACK_SIZE
   ram      (rwx) : ORIGIN = RAM_START_ADDRESS, LENGTH = LENGTH(mcu_ram) - LENGTH(stack) - LENGTH(co_stack) - RAM_START_ADDRESS
   
   udma0   (rwx)  : ORIGIN = 0x2fc00, LENGTH = 0x400
   udma1   (rwx)  : ORIGIN = 0x24060000, LENGTH = 0x400
}

ENTRY(Reset_Handler)
 
SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
		
        *(.text*)        
    	

		KEEP(*(.init))
		KEEP(*(.fini))
		
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
		
		*(.rodata*)
		
		KEEP(*(.eh_fram e*))
	} > mcu_rom 
	
	.ARM.extab : 
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > mcu_rom 
	
	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > mcu_rom 
	
	__exidx_end = .;
	__etext = .;
	
	/* _sidata is used in coide startup code */
	_sidata = __etext;
	
	.data :
	{
		__data_start__ = .;
		
		/* _sdata is used in coide startup code */
		_sdata = __data_start__;
		
		*(vtable)
		*(.data*)

		
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
	} > ram AT> mcu_rom 
	
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
		
	
	.heap (NOLOAD):
	{
		__HeapBase = .;
		*(.heap*)
		. = ABSOLUTE(ORIGIN(ram) + LENGTH(ram));
		__HeapLimit = .;
	} > ram
	
	.co_stack ALIGN(8) (NOLOAD):
	{
		__CoStackLimit = .;
		*(.co_stack .co_stack.*)
		. = ABSOLUTE(ORIGIN(co_stack) + LENGTH(co_stack));
		__co_stackTop = .;
	} > co_stack 
	
	.stack ALIGN(8) (NOLOAD):
	{
		__StackLimit = .;
		*(.stack .stack.*)
		. = ABSOLUTE(ORIGIN(stack) + LENGTH(stack));
		__StackTop = .;
	} > stack
	
	.udma_addr0 :
	{
		*(.udma_addr0*)
	} > udma0 AT> mcu_rom 

	.udma_addr1 :
	{
		*(.udma_addr1*)
	} > udma1 AT> mcu_rom	
			

	
	PROVIDE(__co_stack = __co_stackTop);
	PROVIDE(__stack = __StackTop);
	
	/* Check if data + heap + stack exceeds ram  limit */
	ASSERT(__StackLimit >= __HeapLimit, "region ram  overflowed with stack")
}
