/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Blinky example
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
#include "arch_system.h"
#include "uart.h"
#include "uart_utils.h"
#include "user_periph_setup.h"
#include "gpio.h"



#define LED_OFF_THRESHOLD           10000
#define LED_ON_THRESHOLD            400000

/**
 ****************************************************************************************
 * @brief Blinky test function
 ****************************************************************************************
 */
void blinky_test(void);

/**
 ****************************************************************************************
 * @brief Main routine of the Blinky example
 ****************************************************************************************
 */
int main (void)
{
    system_init();
    blinky_test();
    while(1);
}

void blinky_test(void)
{
    volatile int i=0;

    printf_string(UART, "\n\r\n\r");
    printf_string(UART, "***************\n\r");
    printf_string(UART, "* BLINKY DEMO *\n\r");
    printf_string(UART, "***************\n\r");

    while(1)
    {
        i++;

        if (LED_OFF_THRESHOLD == i)
        {
            GPIO_SetActive(LED_PORT, LED_PIN);
            printf_string(UART, "\n\r *LED ON* ");
        }

        if (LED_ON_THRESHOLD == i)
        {
            GPIO_SetInactive(LED_PORT, LED_PIN);
            printf_string(UART, "\n\r *LED OFF* ");
        }

        if (i == 2 * LED_ON_THRESHOLD)
        {
            i = 0;
        }
    }
}
