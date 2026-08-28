/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief UART example for DA14585, DA14586 and DA14531 devices.
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
#include "user_periph_setup.h"
#include "uart_common.h"
#include "uart_utils.h"
#include "gpio.h"

/**
 ****************************************************************************************
 * @brief UART print example
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_print_example(uart_t * uart);

/**
 ****************************************************************************************
 * @brief Main routine of the UART example
 ****************************************************************************************
 */
int main(void)
{
    system_init();

    // Setup UART1 pins and configuration
    uart_periph_init(UART1);

    // Run UART1 print example
    uart_print_example(UART1);

    // Run UART1 send blocking example
    uart_send_blocking_example(UART1);

    // Run UART1 send interrupt example
    uart_send_interrupt_example(UART1);

#if defined (CFG_UART_DMA_SUPPORT)
    // Run UART1 send dma example
    uart_send_dma_example(UART1);
#endif
    // Run UART1 receive blocking example
    uart_receive_blocking_example(UART1);

    // Run UART1 receive interrupt example
    uart_receive_interrupt_example(UART1);

#if defined (CFG_UART_DMA_SUPPORT)
    // Run UART1 receive dma example
    uart_receive_dma_example(UART1);
#endif

    // Run UART1 loopback example
    uart_loopback_interrupt_example(UART1);


    // Setup UART2 pins and configuration
    uart_periph_init(UART2);

    // Run UART2 print example
    uart_print_example(UART2);

    // Run UART2 send blocking example
    uart_send_blocking_example(UART2);

    // Run UART2 send interrupt example
    uart_send_interrupt_example(UART2);

#if defined (CFG_UART_DMA_SUPPORT)
    // Run UART2 send dma example
    uart_send_dma_example(UART2);
#endif

    // Run UART2 receive blocking example
    uart_receive_blocking_example(UART2);

    // Run UART2 receive interrupt example
    uart_receive_interrupt_example(UART2);

#if defined (CFG_UART_DMA_SUPPORT)
    // Run UART2 receive dma example
    uart_receive_dma_example(UART2);
#endif

    // Run UART2 loopback example
    uart_loopback_interrupt_example(UART2);

    printf_string(UART2, "\n\n\n\r* UART examples finished successfully *\n\r");
    while(1);
}

void uart_print_example(uart_t * uart)
{
    printf_string(uart, "\n\r\n\r****************************************\n\r");
    if (uart == UART1)
    {
        printf_string(uart, "* UART1 Print example *\n\r");
    }
    else
    {
        printf_string(uart, "* UART2 Print example *\n\r");
    }
    printf_string(uart, "****************************************\n\r\n\r");
    printf_string(uart, " printf_string() = Hello World! \n\r");
    printf_string(uart, "\n\r print_hword()   = 0x");
    print_hword(uart, 0xAABB);
    printf_string(uart, "\n\r");
    printf_string(uart, "\n\r print_word()    = 0x");
    print_word(uart, 0x11223344);
    printf_string(uart, "\n\r");
    printf_string(uart, "****************************************\n\r\n\r");
}
