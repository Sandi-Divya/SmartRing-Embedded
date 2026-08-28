/**
 ****************************************************************************************
 *
 * @file uart_send_examples.c
 *
 * @brief UART send examples.
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

#include "uart.h"
#include "uart_utils.h"
#include "uart_common.h"

static const char OUTPUT_STRING[] = "0123456789 0123456789 0123456789 0123456789 0123456789\r\n"
                                    "0123456789 0123456789 0123456789 0123456789 0123456789\r\n"
                                    "0123456789 0123456789 0123456789 0123456789 0123456789\r\n"
                                    "0123456789 0123456789 0123456789 0123456789 0123456789\r\n"
                                    "0123456789 0123456789 0123456789 0123456789 0123456789\r\n"
                                    ;

volatile bool uart_send_finished = false;
volatile uint16_t data_sent_cnt = 0;

static void uart_send_cb(uint16_t length)
{
    data_sent_cnt = length;
    uart_send_finished = true;
}

void uart_send_blocking_example(uart_t* uart)
{
    printf_string(uart, "\n\r\n\r****************************************\n\r");
    if (uart == UART1)
    {
        printf_string(uart, "* UART1 Send Blocking example *\n\r");
    }
    else
    {
        printf_string(uart, "* UART2 Send Blocking example *\n\r");
    }
    printf_string(uart, "****************************************\n\r\n\r");

    uart_send(uart, (uint8_t *)OUTPUT_STRING, sizeof(OUTPUT_STRING) - 1, UART_OP_BLOCKING);

    printf_string(uart, "****************************************\n\r\n\r");
}

void uart_send_interrupt_example(uart_t* uart)
{
    printf_string(uart, "\n\r\n\r****************************************\n\r");
    if (uart == UART1)
    {
        printf_string(uart, "* UART1 Send Interrupt example *\n\r");
    }
    else
    {
        printf_string(uart, "* UART2 Send Interrupt example *\n\r");
    }
    printf_string(uart, "****************************************\n\r\n\r");

    uart_send_finished = false;
    data_sent_cnt = 0;

    uart_register_tx_cb(uart, uart_send_cb);
    uart_send(uart, (uint8_t *)OUTPUT_STRING, sizeof(OUTPUT_STRING) - 1, UART_OP_INTR);
    while (!uart_send_finished);
    printf_string(uart, "\n\rData sent: 0x");
    print_hword(uart, data_sent_cnt);
    printf_string(uart, " Bytes\n\r\n\r");
    printf_string(uart, "****************************************\n\r\n\r");
}

#if defined (CFG_UART_DMA_SUPPORT)
void uart_send_dma_example(uart_t* uart)
{
    printf_string(uart, "\n\r\n\r****************************************\n\r");
    if (uart == UART1)
    {
        printf_string(uart, "* UART1 Send DMA example *\n\r");
    }
    else
    {
        printf_string(uart, "* UART2 Send DMA example *\n\r");
    }
    printf_string(uart, "****************************************\n\r\n\r");

    uart_send_finished = false;
    data_sent_cnt = 0;

    uart_register_tx_cb(uart, uart_send_cb);
    uart_send(uart, (uint8_t *)OUTPUT_STRING, sizeof(OUTPUT_STRING) - 1, UART_OP_DMA);
    while (!uart_send_finished);
    printf_string(uart, "\n\rData sent: 0x");
    print_hword(uart, data_sent_cnt);
    printf_string(uart, " Bytes\n\r\n\r");
    printf_string(uart, "****************************************\n\r\n\r");
}
#endif
