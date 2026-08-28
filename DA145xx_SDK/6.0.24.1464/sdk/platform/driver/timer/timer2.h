/**
 ****************************************************************************************
 * @addtogroup Drivers
 * @{
 * @addtogroup Timers
 * @{
 * @addtogroup Timer2
 * @brief Timer2 driver API
 * @{
 *
 * @file timer2.h
 *
 * @brief DA14585/DA14586/DA14531 Timer2 driver header file.
 *
 * @note The Timer0/Timer2 input clock shall have been enabled first.
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

#ifndef _TIMER2_H_
#define _TIMER2_H_

#include <stdint.h>
#include "datasheet.h"

#if defined (__DA14531__)
/// Timer2 clock source
typedef enum
{
    /// Timer2 uses the LP clock
    TIM2_CLK_LP  = 0,

    /// Timer2 uses the system clock
    TIM2_CLK_SYS = 1
} tim2_clk_src_t;
#endif

/// Timer2 pause (by HW)
typedef enum
{
    /// HW cannot pause Timer2
    TIM2_HW_PAUSE_OFF = 0,

    /// HW can pause Timer2
    TIM2_HW_PAUSE_ON  = 1
} tim2_hw_pause_t;

/// Timer2 pause (by SW)
typedef enum
{
    /// SW resumes Timer2 run
    TIM2_SW_PAUSE_OFF = 0,

    /// SW pauses Timer2 run
    TIM2_SW_PAUSE_ON  = 1
} tim2_sw_pause_t;

/// Timer2 disable/enable
typedef enum
{
    /// Timer2 disabled
    TIM2_OFF = 0,

    /// Timer2 enabled
    TIM2_ON  = 1
} tim2_t;

/// Timer2 PWM signals
typedef enum
{
    /// Timer2 PWM2 signal
    TIM2_PWM_2 = 1,

    /// Timer2 PWM3 signal
    TIM2_PWM_3 = 2,

    /// Timer2 PWM4 signal
    TIM2_PWM_4 = 3,

#if defined (__DA14531__)
    /// Timer2 PWM5 signal
    TIM2_PWM_5 = 4,

    /// Timer2 PWM6 signal
    TIM2_PWM_6 = 5,

    /// Timer2 PWM7 signal
    TIM2_PWM_7 = 6
#endif
} tim2_pwm_t;

/// Timer2 configuration
typedef struct
{
#if defined (__DA14531__)
    /// Timer2 input clock frequency
    tim2_clk_src_t clk_source;
#endif

    /// Timer2 pause (by HW)
    tim2_hw_pause_t hw_pause;
} tim2_config_t;

/// Timer2 PWM signal configuration
typedef struct
{
    /// PWM signal to be configured
    tim2_pwm_t pwm_signal;

    /// Duty cycle of the PWM signal (0% - 100%)
    uint8_t pwm_dc;

#if defined (__DA14531__)
    /// Offset of the PWM signal (0% - 100%)
    uint8_t pwm_offset;
#endif
} tim2_pwm_config_t;

/**
 ****************************************************************************************
 * @brief Configures Timer2 according to @p config.
 * @param[in] config The timer configuration.
 ****************************************************************************************
 */
void timer2_config(tim2_config_t *config);

/**
 ****************************************************************************************
 * @brief Sets Timer2 output frequency of the PWM signals to the desired one.
 * @note Timer2 output PWM frequency (as for DA14585/DA14586, sys_clk is the
 * ONLY Timer2 input clock source) may be within the following range:
 * - [2 Hz, 16 kHz], if Timer2 input clock frequency is 32 kHz,
 * - [123 Hz, 1 MHz], if Timer2 input clock frequency is 2 MHz,
 * - [245 Hz, 2 MHz], if Timer2 input clock frequency is 4 MHz,
 * - [489 Hz, 4 MHz], if Timer2 input clock frequency is 8 MHz,
 * - [977 Hz, 8 MHz], if Timer2 input clock frequency is 16 MHz.
 * In general, Timer2 output PWM frequency may be within the following range:
 * - minimum = (Timer2 input clock frequency) / (2^14 - 1),
 * - maximum = (Timer2 input clock frequency) / 2.
 * @param[in] pwm_freq The desired frequency of the PWM signals in Hz.
 * @param[in] input_freq The Timer2 input clock frequency in Hz.
 ****************************************************************************************
 */
void timer2_pwm_freq_set(uint32_t pwm_freq, uint32_t input_freq);

/**
 ****************************************************************************************
 * @brief Configures one of the available PWM signals according to @p pwm_config.
 * @param[in] pwm_config The PWM signal configuration.
 ****************************************************************************************
 */
void timer2_pwm_signal_config(tim2_pwm_config_t *pwm_config);

/**
 ****************************************************************************************
 * @brief Enables the Timer2 PWM signals.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer2_start(void)
{
    SetBits16(TRIPLE_PWM_CTRL_REG, TRIPLE_PWM_ENABLE, TIM2_ON);
}

/**
 ****************************************************************************************
 * @brief Disables the Timer2 PWM signals.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer2_stop(void)
{
    SetBits16(TRIPLE_PWM_CTRL_REG, TRIPLE_PWM_ENABLE, TIM2_OFF);
}

/**
 ****************************************************************************************
 * @brief Pauses the Timer2 PWM signals in the SW context.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer2_pause(void)
{
    SetBits16(TRIPLE_PWM_CTRL_REG, SW_PAUSE_EN, TIM2_SW_PAUSE_ON);
}

/**
 ****************************************************************************************
 * @brief Resumes the Timer2 PWM signals in the SW context.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer2_resume(void)
{
    SetBits16(TRIPLE_PWM_CTRL_REG, SW_PAUSE_EN, TIM2_SW_PAUSE_OFF);
}

#endif /* _TIMER2_H_ */

///@}
///@}
///@}
