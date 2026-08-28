/**
 ****************************************************************************************
 * @addtogroup Drivers
 * @{
 * @addtogroup Timers
 * @{
 * @addtogroup Timer0_2 Timer0/2
 * @brief Timer0/2 driver API - common part
 * @{
 *
 * @file timer0_2.h
 *
 * @brief DA14585/DA14586/DA14531 Timer0/Timer2 driver header file - common part.
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

#ifndef _TIMER0_2_H_
#define _TIMER0_2_H_

#include "datasheet.h"

/// Timer0/Timer2 input clock division factor (provided that system clock is the input clock)
typedef enum
{
    /// System clock divided by 1
    TIM0_2_CLK_DIV_1 = 0,

    /// System clock divided by 2
    TIM0_2_CLK_DIV_2 = 1,

    /// System clock divided by 4
    TIM0_2_CLK_DIV_4 = 2,

    /// System clock divided by 8
    TIM0_2_CLK_DIV_8 = 3
} tim0_2_clk_div_t;

/// Timer0/Timer2 input clock disable/enable
typedef enum
{
    /// Timer0/Timer2 input clock disabled
    TIM0_2_CLK_OFF,

    /// Timer0/Timer2 input clock enabled
    TIM0_2_CLK_ON,
} tim0_2_clk_t;

/// Configuration bearing the Timer0/Timer2 input clock division factor
typedef struct
{
    /// Timer0/Timer2 input clock division factor
    tim0_2_clk_div_t clk_div;
} tim0_2_clk_div_config_t;

/**
 ****************************************************************************************
 * @brief Enables the Timer0/Timer2 input clock.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer0_2_clk_enable(void)
{
    SetBits16(CLK_PER_REG, TMR_ENABLE, TIM0_2_CLK_ON);
}

/**
 ****************************************************************************************
 * @brief Disables the Timer0/Timer2 input clock.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer0_2_clk_disable(void)
{
    SetBits16(CLK_PER_REG, TMR_ENABLE, TIM0_2_CLK_OFF);
}

/**
 ****************************************************************************************
 * @brief Sets the Timer0/Timer2 input clock division factor.
 * @param[in] clk_div_config The configuration bearing the division factor.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer0_2_clk_div_set(tim0_2_clk_div_config_t *clk_div_config)
{
    SetBits16(CLK_PER_REG, TMR_DIV, clk_div_config->clk_div);
}

#endif /* _TIMER0_2_H_ */

///@}
///@}
///@}
