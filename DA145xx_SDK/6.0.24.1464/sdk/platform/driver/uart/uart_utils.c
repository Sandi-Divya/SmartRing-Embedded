/**
 ****************************************************************************************
 *
 * @file uart_utils.c
 *
 * @brief UART utilities
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

#include <string.h>
#include "uart_utils.h"

/**
 ****************************************************************************************
 * @brief Reverse string s in place
 * @param[in,out] s        String to reverse
 ****************************************************************************************
 */
static void reverse(char s[])
 {
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
 }

/**
 ****************************************************************************************
 * @brief convert n to characters in s
 * @param[in] n            integer to convert
 * @param[in] s            converted character
 ****************************************************************************************
 */
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

void printf_byte(uart_t *uart, uint8_t ch)
{
    // print a Byte in hex format
    char b;
    b = ((0xF0 & ch) >> 4);
    b += (b < 10) ? 48 : 55;

    uart_send(uart, (uint8_t *)&b, 1, UART_OP_BLOCKING);
    b = (0xF & ch);
    b += (b < 10) ? 48 : 55;
    uart_send(uart, (uint8_t *)&b, 1, UART_OP_BLOCKING);
}

void printf_string(uart_t *uart, char *str)
{
    uart_send(uart, (uint8_t *) str, strlen(str), UART_OP_BLOCKING);
}

void print_hword(uart_t *uart, uint16_t aHalfWord)
{
    printf_byte(uart, (aHalfWord >> 8) & 0xFF);
    printf_byte(uart, (aHalfWord) & 0xFF);
}

void print_word(uart_t *uart, uint32_t aWord)
{
    printf_byte(uart, (aWord >> 24) & 0xFF);
    printf_byte(uart, (aWord >> 16) & 0xFF);
    printf_byte(uart, (aWord >> 8) & 0xFF);
    printf_byte(uart, (aWord) & 0xFF);
}

void printf_byte_dec(uart_t *uart, int a)
{
    char temp_buf[4];

    if (a > 255)
    return;

    itoa(a, temp_buf);
    printf_string(uart, temp_buf);
}

void printf_word_dec(uart_t *uart, int a)
{
    char temp_buf[6];

    itoa(a, temp_buf);
    printf_string(uart, temp_buf);
}
