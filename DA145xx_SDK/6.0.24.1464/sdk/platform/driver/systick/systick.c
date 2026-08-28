/**
 ****************************************************************************************
 *
 * @file systick.c
 *
 * @brief SysTick driver.
 *
 * Copyright (C) 2012-2025 Renesas Electronics Corporation and/or its affiliates.
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

#include <stdio.h>
#include "compiler.h"
#include "systick.h"
#include "datasheet.h"

/// Callback function to be called within SysTick_Handler.
static systick_callback_function_t callback_function __SECTION_ZERO("retention_mem_area0");

void systick_register_callback(systick_callback_function_t callback)
{
    callback_function = callback;
}

#if !defined (__EXCLUDE_SYSTICK_HANDLER__)
void SysTick_Handler(void)
{
    if (callback_function != NULL)
        callback_function();
}
#endif

void systick_start(uint32_t usec, uint8_t exception)
{
    SetBits32(&SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, 0);          // disable systick
    SetBits32(&SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, usec-1);     // set systick timeout based on 1MHz clock
    SetBits32(&SysTick->VAL,  SysTick_VAL_CURRENT_Msk, 0);          // clear the Current Value Register and the COUNTFLAG to 0
    SetBits32(&SysTick->CTRL, SysTick_CTRL_TICKINT_Msk, exception); // generate or not the SysTick exception
    SetBits32(&SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk, 0);       // use a reference clock frequency of 1 MHz
    SetBits32(&SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, 1);          // enable systick
}

void systick_stop(void)
{
    SetBits32(&SysTick->VAL,  SysTick_VAL_CURRENT_Msk, 0);            // clear the Current Value Register and the COUNTFLAG to 0
    SetBits32(&SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, 0);            // disable systick
}

void systick_wait(uint32_t usec)
{
    systick_start(usec, false);

    // wait for the counter flag to become 1
    // Because the flag is cleared automatically when the register is read, there is no need to clear it
    while (!GetBits32(&SysTick->CTRL, SysTick_CTRL_COUNTFLAG_Msk));

    systick_stop();
}

uint32_t systick_value(void)
{
    return GetBits32(&SysTick->VAL, SysTick_VAL_CURRENT_Msk);
}
