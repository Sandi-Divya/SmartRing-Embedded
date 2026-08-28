;/**
; ****************************************************************************************
; *
; * @file pulse.s
; *
; * @brief
; *
; * Copyright (C) 2017-2023 Renesas Electronics Corporation and/or its affiliates.
; * All rights reserved. Confidential Information.
; *
; * This software ("Software") is supplied by Renesas Electronics Corporation and/or its
; * affiliates ("Renesas"). Renesas grants you a personal, non-exclusive, non-transferable,
; * revocable, non-sub-licensable right and license to use the Software, solely if used in
; * or together with Renesas products. You may make copies of this Software, provided this
; * copyright notice and disclaimer ("Notice") is included in all such copies. Renesas
; * reserves the right to change or discontinue the Software at any time without notice.
; *
; * THE SOFTWARE IS PROVIDED "AS IS". RENESAS DISCLAIMS ALL WARRANTIES OF ANY KIND,
; * WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
; * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. TO THE
; * MAXIMUM EXTENT PERMITTED UNDER LAW, IN NO EVENT SHALL RENESAS BE LIABLE FOR ANY DIRECT,
; * INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING FROM, OUT OF OR IN
; * CONNECTION WITH THE SOFTWARE, EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY OF
; * SUCH DAMAGES. USE OF THIS SOFTWARE MAY BE SUBJECT TO TERMS AND CONDITIONS CONTAINED IN
; * AN ADDITIONAL AGREEMENT BETWEEN YOU AND RENESAS. IN CASE OF CONFLICT BETWEEN THE TERMS
; * OF THIS NOTICE AND ANY SUCH ADDITIONAL LICENSE AGREEMENT, THE TERMS OF THE AGREEMENT
; * SHALL TAKE PRECEDENCE. BY CONTINUING TO USE THIS SOFTWARE, YOU AGREE TO THE TERMS OF
; * THIS NOTICE.IF YOU DO NOT AGREE TO THESE TERMS, YOU ARE NOT PERMITTED TO USE THIS
; * SOFTWARE.
; *
; ****************************************************************************************
; */

GUARD_COUNTER_INIT  EQU 3000000 ; 3000000 iterations x 9 cycles/iteration * 62.5 ns/cycle = 1.6875 seconds

SYSTICK_CTRL_ADDR   EQU 0xE000E010 ; (R/W)  SysTick Control and Status Register
SYSTICK_LOAD_ADDR   EQU 0xE000E014 ; (R/W)  SysTick Reload Value Register
SYSTICK_VAL_ADDR    EQU 0xE000E018 ; (R/W)  SysTick Current Value Register
SYSTICK_CALIB_ADDR  EQU 0xE000E01C ; (R/ )  SysTick Calibration Register


                THUMB

                AREA    |.text|, CODE, READONLY


measure_pulse   PROC
                EXPORT  measure_pulse

                push {r1-r5, lr}

                LDR      r5, =GUARD_COUNTER_INIT ; guard counter initialization
wait_low
                SUBS     r5, r5, #1              ; decrement guard counter 
                BEQ      abort                   ; abort if guard counter has reached zero
                LDRH     r2,[r0]
                TST      r2, r1
                BNE      wait_low

                ; detect positive edge
wait_high
                SUBS     r5, r5, #1              ; decrement guard counter 
                BEQ      abort                   ; abort if guard counter has reached zero
                LDRH     r2, [r0]
                TST      r2, r1
                BEQ      wait_high

                ; start systick timer
                LDR      r2, =SYSTICK_CTRL_ADDR
                LDR      r3, [r2]
                MOVS     r4, #1
                ORRS     r3,r3,r4
                STR      r3, [r2]

                ; detect negative edge
while_high
                SUBS     r5, r5, #1              ; decrement guard counter 
                BEQ      abort                   ; abort if guard counter has reached zero
                LDRH     r2,[r0]
                TST      r2, r1
                BNE      while_high

                ; read and return current systick value
                LDR      r2, =SYSTICK_VAL_ADDR
                LDR      r0, [r2]
                POP      {r1-r5, pc}

abort
                MOVS    r0, #0
                POP     {r1-r5, pc}
                ALIGN

                ENDP

                END
