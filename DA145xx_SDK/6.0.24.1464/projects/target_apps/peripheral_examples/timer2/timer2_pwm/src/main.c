/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief DA14585/DA14586 Timer2 example.
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
#include <stdint.h>
#include "arch_system.h"
#include "uart.h"
#include "uart_utils.h"
#include "user_periph_setup.h"
#include "timer0_2.h"
#include "timer2.h"

/**
 ****************************************************************************************
 * @brief Timer2 test function
 ****************************************************************************************
 */
void timer2_test(void);

/**
 ****************************************************************************************
 * @brief Simple delay function
 ****************************************************************************************
 */
void simple_delay(void);


static tim0_2_clk_div_config_t clk_div_config =
{
    .clk_div  = TIM0_2_CLK_DIV_8
};

static tim2_config_t config =
{
    .hw_pause = TIM2_HW_PAUSE_OFF,
#if defined (__DA14531__)
    .clk_source = TIM2_CLK_SYS
#endif
};

static tim2_pwm_config_t pwm_config;

/**
 ****************************************************************************************
 * @brief Main routine of the timer2 example
 ****************************************************************************************
 */
int main (void)
{
    system_init();
    timer2_test();
    while(1);
}

void timer2_test(void)
{
    uint8_t i;
    printf_string(UART, "\n\r\n\r***************");
    printf_string(UART, "\n\r* TIMER2 TEST *\n\r");
    printf_string(UART, "***************\n\r");

    // Enable the Timer0/Timer2 input clock
    timer0_2_clk_enable();
    // Set the Timer0/Timer2 input clock division factor
    timer0_2_clk_div_set(&clk_div_config);

    timer2_config(&config);

    // System clock, divided by 8, is the Timer2 input clock source (according
    // to the clk_div_config struct above).
    timer2_pwm_freq_set(PWM_FREQUENCY, 16000000 / 8);

    timer2_start();

    printf_string(UART, "\n\rTIMER2 started!");
    for (i = 0; i < 100; i += 10)
    {
        // Set PWM2 duty cycle
        pwm_config.pwm_signal = TIM2_PWM_2;
        pwm_config.pwm_dc     = i;
        timer2_pwm_signal_config(&pwm_config);
        
        // Set PWM3 duty cycle
        pwm_config.pwm_signal = TIM2_PWM_3;
        pwm_config.pwm_dc     = i + 5;
        timer2_pwm_signal_config(&pwm_config);
        
        // Set PWM4 duty cycle
        pwm_config.pwm_signal = TIM2_PWM_4;
        pwm_config.pwm_dc     = i + 10;
        timer2_pwm_signal_config(&pwm_config);
        
        // Release sw pause to let PWM2, PWM3, and PWM4 run
        timer2_resume();

        // Delay approx. 1 second to observe duty cycle change
        simple_delay();

        timer2_pause();
    }

    timer2_stop();

    // Disable the Timer0/Timer2 input clock
    timer0_2_clk_disable();

    printf_string(UART, "\n\rTIMER2 stopped!\n\r");
    printf_string(UART, "\n\rEnd of test\n\r");
}

void simple_delay(void)
{
    volatile uint32_t i;

    for (i = 0xFFFFF; i != 0; --i)
        ;
}
