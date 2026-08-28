/**
 ****************************************************************************************
 *
 * @file uart_common.h
 *
 * @brief UART examples common header file.
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

#ifndef _UART_COMMON_H
#define _UART_COMMON_H

#include "uart.h"

// Send examples functions

/**
 ****************************************************************************************
 * @brief UART send in blocking mode example.
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_send_blocking_example(uart_t* uart);

/**
 ****************************************************************************************
 * @brief UART send in interrupt mode example.
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_send_interrupt_example(uart_t* uart);

/**
 ****************************************************************************************
 * @brief UART send in DMA mode example.
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_send_dma_example(uart_t* uart);

// Receive examples functions
/**
 ****************************************************************************************
 * @brief UART receive in blocking mode example.
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_receive_blocking_example(uart_t* uart);

/**
 ****************************************************************************************
 * @brief UART receive in interrupt mode example.
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_receive_interrupt_example(uart_t* uart);

/**
 ****************************************************************************************
 * @brief UART receive in DMA mode example.
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_receive_dma_example(uart_t* uart);

// Loopback examples functions

/**
 ****************************************************************************************
 * @brief UART Interrupt loopback example
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_loopback_interrupt_example(uart_t *uart);


#endif // _UART2_PRINT_STRING_H
