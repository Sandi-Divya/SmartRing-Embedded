/**
 ****************************************************************************************
 *
 * @file programmer_DA14531_jtag.lds.S
 *
 * @brief GNU LD linker script file.
 *
 * Copyright (C) 2018-2025 Renesas Electronics Corporation and/or its affiliates.
 * All rights reserved. Confidential Information.
 *
 * This software ("Software") is supplied by Renesas Electronics Corporation and/or its
 * affiliates ("Renesas"). Renesas grants you a personal, non-exclusive, non-transferable,
 * revocable, non-sub-licensable right and license to use the Software, solely if used in
 * or together with Renesas products. You may make copies of this Software, provided this
 * copyright notice and disclaimer ("Notice") is included in all such copies. Renesas
 * reserves the right to change or discontinue the Software at any time without notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS". RENESAS DISCLAIMS ALL WARRANTIES OF ANY KIND,
 * WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. TO THE
 * MAXIMUM EXTENT PERMITTED UNDER LAW, IN NO EVENT SHALL RENESAS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE, EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES. USE OF THIS SOFTWARE MAY BE SUBJECT TO TERMS AND CONDITIONS CONTAINED IN
 * AN ADDITIONAL AGREEMENT BETWEEN YOU AND RENESAS. IN CASE OF CONFLICT BETWEEN THE TERMS
 * OF THIS NOTICE AND ANY SUCH ADDITIONAL LICENSE AGREEMENT, THE TERMS OF THE AGREEMENT
 * SHALL TAKE PRECEDENCE. BY CONTINUING TO USE THIS SOFTWARE, YOU AGREE TO THE TERMS OF
 * THIS NOTICE.IF YOU DO NOT AGREE TO THESE TERMS, YOU ARE NOT PERMITTED TO USE THIS
 * SOFTWARE.
 *
 ****************************************************************************************
 */
 
#ifndef __DA14531__
#define __DA14531__
#endif 
#include "sdk_defs.h"



MEMORY
{
    LR_1 (rwx) : ORIGIN = SDK_RAM_1_BASE_ADDR, LENGTH = 0x07FC3C00 - SDK_RAM_1_BASE_ADDR - SDK_FLASH_PROGRAMMER_STACK_SIZE
    STACK : ORIGIN = 0x07FC3C00 - SDK_FLASH_PROGRAMMER_STACK_SIZE, LENGTH = SDK_FLASH_PROGRAMMER_STACK_SIZE
    RAM_RESERVED (rw) : ORIGIN = 0x07FC3C00, LENGTH = SDK_RAM_END_ADDR - 0x07FC3C00
}

ENTRY(Reset_Handler)

SECTIONS
{
    ER_1 :
    {
        KEEP(*(.vectors))
        KEEP(*(startup_DA14531.o))
        *(InRoot$$Sections)
        KEEP(*(system_DA1431.o))
        .=.
        *(.text*)
        *(.rodata*)
    } > LR_1

    __etext = ALIGN(4);
    .data :
    {
        . = ALIGN(4);
        __data_start__ = .;

        . = ALIGN(4);
        KEEP(*(.init))
        KEEP(*(.fini))

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

        KEEP(*(.eh_frame*))

        . = ALIGN(4);
        __data_end__ = .;
    } > LR_1
    
    __data_size = SIZEOF(.data);
    __data_start = ADDR(.data);
    __data_source = LOADADDR(.data);

    ER2 :
    {
        . = ALIGN(8);
        KEEP(*(non_init))
    } > LR_1


    __ret_data_end__ = 0;
    __ret_data_start__ = 0;

    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (LOADADDR(.data))
        LONG (__data_start__)
        LONG ((__data_end__ - __data_start__)/4)
        __copy_table_end__ = .;
    } > LR_1

    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG ((__bss_end__ - __bss_start__)/4)
        LONG (__ret_data_start__)
        LONG ((__ret_data_end__ - __ret_data_start__)/4)
        __zero_table_end__ = .;
    } > LR_1

    .bss : // ER_3
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > LR_1
    __bss_start = __bss_start__;
    __bss_size = SIZEOF(.bss);
    
    __tls_base = 0;
    __arm32_tls_tcb_offset = 0;


    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > LR_1

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > LR_1
   
    .stack (NOLOAD) :
    {
        __StackLimit = .;                         /* Bottom of stack */
        . = . + SDK_FLASH_PROGRAMMER_STACK_SIZE;  /* Reserve stack area */
        __StackTop = .;                           /* Top of stack */
    } > STACK

    PROVIDE(__stack = __StackTop);
}

/* Include ROM symbol definitions */
#ifdef __DA14531_01__
INCLUDE da14531_01_for_periph.lds
#else
INCLUDE da14531_for_periph.lds
#endif

