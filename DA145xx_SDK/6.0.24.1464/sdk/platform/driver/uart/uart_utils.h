/**
 ****************************************************************************************
 * @addtogroup Drivers
 * @{
 * @addtogroup UART
 * @{
 * @addtogroup UART_UTILS UART Driver Utilities
 * @brief UART driver utilities API
 * @{
 *
 * @file uart_utils.h
 *
 * @brief UART utilities header file.
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

#ifndef _UART_UTILS_H_
#define _UART_UTILS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "uart.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief print a Byte in hex format
 * @param[in] uart              Identifies which UART to use
 * @param[in] ch                Character to print
 ****************************************************************************************
 */
void printf_byte(uart_t *uart, uint8_t ch);

/**
 ****************************************************************************************
 * @brief UART print string function
 * @param[in] uart              Identifies which UART to use
 * @param[in] str               String to print
 ****************************************************************************************
 */
void printf_string(uart_t *uart, char *str);

/**
 ****************************************************************************************
 * @brief Prints a (16-bit) half-word in hex format using printf_byte
 * @param[in] uart              Identifies which UART to use
 * @param[in] aHalfWord         16-bit half-word to print
 ****************************************************************************************
 */
void print_hword(uart_t *uart, uint16_t aHalfWord);

 /**
 ****************************************************************************************
 * @brief Prints a (32-bit) word in hex format using printf_byte
 * @param[in] uart              Identifies which UART to use
 * @param[in] aWord             32-bit half-word to print
 ****************************************************************************************
 */
void print_word(uart_t *uart, uint32_t aWord);

 /**
 ****************************************************************************************
 * @brief prints a byte in dec format
 * @param[in] uart              Identifies which UART to use
 * @param[in] a                 Byte to print
 ****************************************************************************************
 */
void printf_byte_dec(uart_t *uart, int a);

 /**
 ****************************************************************************************
 * @brief prints a word in dec format
 * @param[in] uart              Identifies which UART to use
 * @param[in] a                 Word to print
 ****************************************************************************************
 */
void printf_word_dec(uart_t *uart, int a);

#endif //_UART_PRINT_H_

///@}
///@}
///@}
