/**
 ****************************************************************************************
 *
 * @file timer0.c
 *
 * @brief DA14585/DA14586/DA14531 Timer0 driver source file.
 *
 * Copyright (C) 2016-2025 Renesas Electronics Corporation and/or its affiliates.
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

#include <stdint.h>
#include <stddef.h>
#include "compiler.h"
#include "datasheet.h"
#include "timer0.h"

timer0_handler_function_t *TIMER0_callback      __SECTION_ZERO("retention_mem_area0");

void timer0_init(TIM0_CLK_SEL_t tim0_clk_sel, PWM_MODE_t pwm_mode, TIM0_CLK_DIV_t tim0_clk_div)
{
    uint16_t temp = (((uint16_t) pwm_mode) << 3) +
                    (((uint16_t) tim0_clk_div) << 2) +
                    (((uint16_t) tim0_clk_sel) << 1);

    SetWord16(TIMER0_CTRL_REG, temp);

    /* Set Priority for TIM0 Interrupt to level 2. */
    NVIC_SetPriority (SWTIM_IRQn, 2);
}

void timer0_start(void)
{
    /* Timer0 is running. */
    SetBits16(TIMER0_CTRL_REG, TIM0_CTRL, TIM0_CTRL_RUNNING);
}

void timer0_stop(void)
{
    /* Timer0 is off and in reset state. */
    SetBits16(TIMER0_CTRL_REG, TIM0_CTRL, TIM0_CTRL_OFF_RESET);
}

void timer0_release(void)
{
    timer0_stop();
}

void timer0_set(uint16_t pwm_on, uint16_t pwm_high, uint16_t pwm_low)
{
    SetWord16(TIMER0_ON_REG, pwm_on);
    SetWord16(TIMER0_RELOAD_M_REG, pwm_high);
    SetWord16(TIMER0_RELOAD_N_REG, pwm_low);
}

void timer0_enable_irq(void)
{
    NVIC_EnableIRQ(SWTIM_IRQn);
}

void timer0_disable_irq(void)
{
    NVIC_DisableIRQ(SWTIM_IRQn);
}

void timer0_register_callback(timer0_handler_function_t *callback)
{
    TIMER0_callback = callback;
}

#if !defined(__EXCLUDE_SWTIM_HANDLER__)
/**
 ****************************************************************************************
 * @brief SWTIM_Handler. The SWTIM_IRQn interrupt handler
 ****************************************************************************************
 */
void SWTIM_Handler(void)
{
    if (TIMER0_callback != NULL)
        TIMER0_callback();
}
#endif
