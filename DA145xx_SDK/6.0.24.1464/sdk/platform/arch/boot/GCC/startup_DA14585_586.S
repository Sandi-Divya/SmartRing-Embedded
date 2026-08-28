/* File: startup_DA14585_586.S
 * Purpose: startup file for Cortex-M0 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.0
 * Date: 16 August 2013
 */

/**
 ****************************************************************************************
 * Copyright (C) 2011-2013 ARM LIMITED
 * Copyright (C) 2018-2020 Modified by Renesas Electronics Corporation and/or its affiliates.
 *
 *  All rights reserved.
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ARM nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************************
 */

    .syntax	unified
    .arch	armv6-m

    .section .stack
    .align	8
#ifdef __STACK_SIZE
        .equ	Stack_Size, __STACK_SIZE
#else
        .equ	Stack_Size, 0x600
#endif
    .globl	__StackTop
    .globl	__StackLimit
__StackLimit:
    .space	Stack_Size
    .size	__StackLimit, . - __StackLimit
    .align	8
__StackTop:
    .size	__StackTop, . - __StackTop

    .section .heap
    .align	8
#ifdef __HEAP_SIZE
        .equ	Heap_Size, __HEAP_SIZE
#else
        .equ	Heap_Size, 0x100
#endif
    .globl	__HeapBase
    .globl	__HeapLimit
__HeapBase:
    .if	Heap_Size
        .space	Heap_Size
    .endif
    .size	__HeapBase, . - __HeapBase
    .align	8
__HeapLimit:
    .size	__HeapLimit, . - __HeapLimit

/* ISR implementations common to DA14585_586 and Barium */
    .thumb
    .section .isr_impl
    .align	2

    .thumb_func
	.weak	Reset_Handler
	.type	Reset_Handler, %function
    .globl  Reset_Handler
    .fnstart

@ Reset Handler
Reset_Handler:
    bl      SystemInit

/*
 * Copy table initialization.
 *
 */
    ldr      r4, =__copy_table_start__
    ldr      r5, =__copy_table_end__

.L_loop0:
    cmp      r4, r5
    bge      .L_loop0_done
    ldr      r1, [r4]                /* source address */
    ldr      r2, [r4, #4]            /* destination address */
    ldr      r3, [r4, #8]            /* word count */
    lsls     r3, r3, #2              /* byte count */

.L_loop0_0:
    subs     r3, #4                  /* decrement byte count */
    blt      .L_loop0_0_done
    ldr      r0, [r1, r3]
    str      r0, [r2, r3]
    b        .L_loop0_0

.L_loop0_0_done:
    adds     r4, #12
    b        .L_loop0

.L_loop0_done:

/*
 * Zero table initialization.
 *
 */
    ldr     r3, =__zero_table_start__
    ldr     r4, =__zero_table_end__

.L_loop2:
    cmp     r3, r4
    bge     .L_loop2_done
    ldr     r1, [r3]
    ldr     r2, [r3, #4]
    movs    r0, 0

.L_loop2_0:
    subs    r2, #4
    blt     .L_loop2_0_done
    str     r0, [r1, r2]
    b       .L_loop2_0

.L_loop2_0_done:
    adds    r3, #8
    b       .L_loop2

.L_loop2_done:

    bl      _start

	.fnend
	.size	Reset_Handler, . - Reset_Handler

	.thumb_func
	.type	NMI_Handler, %function
	.weak	NMI_Handler
    .fnstart

@ NMI handler
NMI_Handler:
    movs    r0, #4
    mov     r1, lr
    tst     r0, r1
    beq     NMI_stacking_used_MSP
    mrs     r0, psp
    ldr     r1, =NMI_HandlerC
    bx      r1
NMI_stacking_used_MSP:
    mrs     r0, msp
    ldr     r1, =NMI_HandlerC
    bx      r1
    .fnend
    .size	NMI_Handler, . - NMI_Handler

	.thumb_func
	.globl	HardFault_Handler
	.type	HardFault_Handler, %function
    .fnstart

@ HardFault handler
HardFault_Handler:
    movs    r0, #4
    mov     r1, lr
    tst     r0, r1
    beq     HardFault_stacking_used_MSP
    mrs     r0, psp
    ldr     r1, =HardFault_HandlerC
    bx      r1
HardFault_stacking_used_MSP:
    mrs     r0, msp
    ldr     r1, =HardFault_HandlerC
    bx      r1
    .fnend
    .size	HardFault_Handler, . - HardFault_Handler

	.thumb
	.thumb_func
	.weak	SVC_Handler
	.type	SVC_Handler, %function
    .fnstart

@ SVC handler
SVC_Handler:
    .weak   SVC_Handler
    b       .
    .fnend
    .size	SVC_Handler, . - SVC_Handler

.end
