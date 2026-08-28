/**
 ****************************************************************************************
 *
 * @file timer2.c
 *
 * @brief DA14585/DA14586/DA14531 Timer2 driver source file.
 *
 * Copyright (C) 2019-2025 Renesas Electronics Corporation and/or its affiliates.
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
#include "timer2.h"
#include "datasheet.h"
#include "arch.h"

void timer2_config(tim2_config_t *config)
{
#if defined (__DA14531__)
    SetBits16(TRIPLE_PWM_CTRL_REG, TRIPLE_PWM_CLK_SEL, config->clk_source);
#endif
    SetBits16(TRIPLE_PWM_CTRL_REG, HW_PAUSE_EN, config->hw_pause);
}

void timer2_pwm_freq_set(uint32_t pwm_freq, uint32_t input_freq)
{
    SetWord16(TRIPLE_PWM_FREQUENCY, (input_freq / pwm_freq) - 1u);
}

void timer2_pwm_signal_config(tim2_pwm_config_t *pwm_config)
{
    ASSERT_WARNING(pwm_config->pwm_dc <= 100u);

    uint32_t counter, dc;

    counter = GetWord16(TRIPLE_PWM_FREQUENCY) + 1u;
    dc      = (counter * (pwm_config->pwm_dc)) / 100u;
#if defined (__DA14531__)
    ASSERT_WARNING(pwm_config->pwm_offset <= 100u);
    uint32_t offset  = (counter * (pwm_config->pwm_offset)) / 100u;
#endif

    switch (pwm_config->pwm_signal)
    {
        case TIM2_PWM_2:
        {
#if defined (__DA14531__)
            SetWord16(PWM2_START_CYCLE, offset);
            SetWord16(PWM2_END_CYCLE, dc + offset);
#else
            SetWord16(PWM2_DUTY_CYCLE, dc);
#endif
            break;
        }

        case TIM2_PWM_3:
        {
#if defined (__DA14531__)
            SetWord16(PWM3_START_CYCLE, offset);
            SetWord16(PWM3_END_CYCLE, dc + offset);
#else
            SetWord16(PWM3_DUTY_CYCLE, dc);
#endif
            break;
        }

        case TIM2_PWM_4:
        {
#if defined (__DA14531__)
            SetWord16(PWM4_START_CYCLE, offset);
            SetWord16(PWM4_END_CYCLE, dc + offset);
#else
            SetWord16(PWM4_DUTY_CYCLE, dc);
#endif
            break;
        }

#if defined (__DA14531__)
        case TIM2_PWM_5:
        {
            SetWord16(PWM5_START_CYCLE, offset);
            SetWord16(PWM5_END_CYCLE, dc + offset);
            break;
        }

        case TIM2_PWM_6:
        {
            SetWord16(PWM6_START_CYCLE, offset);
            SetWord16(PWM6_END_CYCLE, dc + offset);
            break;
        }

        case TIM2_PWM_7:
        {
            SetWord16(PWM7_START_CYCLE, offset);
            SetWord16(PWM7_END_CYCLE, dc + offset);
            break;
        }
#endif

        default:
        {
            /* Default case added for the sake of completeness; it will
               never be reached. */
            break;
        }
    }
}
