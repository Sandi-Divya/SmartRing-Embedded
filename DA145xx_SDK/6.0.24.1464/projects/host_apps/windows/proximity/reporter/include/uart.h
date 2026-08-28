/**
 ****************************************************************************************
 *
 * @file uart.h
 *
 * @brief Definitions for uart interface.
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

#ifndef _UART_H_
#define _UART_H_

#define MAX_PACKET_LENGTH 350
#define MIN_PACKET_LENGTH 9
 /*
 ****************************************************************************************
 * @brief Write message to UART.
 *
 *  @param[in] size  Message's size.
 *  @param[in] data  Pointer to message's data.
 ****************************************************************************************
*/
void UARTSend(unsigned short size, unsigned char *data);
/*
 ****************************************************************************************
 * @brief Send message received from UART to application's main thread.
 *
 *  @param[in] length           Message's size.
 *  @param[in] bInputDataPtr    Pointer to message's data.
 ****************************************************************************************
*/
void SendToMain(unsigned short length, uint8_t *bInputDataPtr);
/*
 ****************************************************************************************
 * @brief UART Reception thread loop.
 ****************************************************************************************
*/
void UARTProc(PVOID unused);

/*
 ****************************************************************************************
 * @brief Init UART iface.
 *
 *  @param[in] Port         COM prot number.
 *  @param[in] BaudRate     Baud rate.
 *
 * @return -1 on failure / 0 on success.
 ****************************************************************************************
*/
uint8_t InitUART(int Port, int BaudRate);


#endif /* _UART_H_ */
