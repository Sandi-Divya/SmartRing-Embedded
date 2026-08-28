/**
 ****************************************************************************************
 *
 * @file console.h
 *
 * @brief Header file for basic console user interface of the host application.
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

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "rwble_config.h"
#include "stdbool.h"

extern bool console_main_pass_state;
extern bool conn_num_flag;

typedef struct {
    unsigned char type;
    unsigned char val;
    unsigned char idx;
} console_msg;

enum
{
    CONSOLE_DEV_DISC_CMD,
    CONSOLE_CONNECT_CMD,
    CONSOLE_DISCONNECT_CMD,
    CONSOLE_RD_LLV_CMD,
    CONSOLE_RD_TXP_CMD,
    CONSOLE_WR_LLV_CMD,
    CONSOLE_WR_IAS_CMD,
    CONSOLE_EXIT_CMD,
};

/*
 ****************************************************************************************
 * @brief Sends Discover devices message to application 's main thread.
 ****************************************************************************************
 */
void ConsoleSendScan(void);

/*
 ****************************************************************************************
 * @brief Sends Connect to device message to application 's main thread.
 *
 *  @param[in] indx Device's index in discovered devices list.
 ****************************************************************************************
 */
void ConsoleSendConnnect(int indx);

/*
 ****************************************************************************************
 * @brief Sends Disconnect request message to application 's main thread.
 *
 *  @param[in] id Selected device id.
 ****************************************************************************************
 */
void ConsoleSendDisconnnect(int id);

/*
 ****************************************************************************************
 * @brief Sends Read request message to application 's main thread.
 *
 *  @param[in] type  Attribute type to read.
 *  @param[in] idx  Selected device id.
 ****************************************************************************************
 */
void ConsoleRead(unsigned char type,unsigned char idx);

/*
 ****************************************************************************************
 * @brief Sends write request message to application 's main thread.
 *
 *  @param[in] type  Attribute type to write.
 *  @param[in] val   Attribute value.
 *  @param[in] idx   Selected device id.
 ****************************************************************************************
 */
void ConsoleWrite(unsigned char type, unsigned char val, unsigned char idx);

/**
 ****************************************************************************************
 * @brief Sends a message to application 's main thread to exit.
 ****************************************************************************************
 */
void ConsoleSendExit(void);

/**
 ****************************************************************************************
 * @brief Handles keypress events and sends corresponding message to application's main thread.
 ****************************************************************************************
 */
void HandleKeyEvent(int Key);

/**
 ****************************************************************************************
 * @brief Demo application Console Menu header
 ****************************************************************************************
 */
void ConsoleTitle(void);

/**
 ****************************************************************************************
 * @brief Prints Console menu in Scan state.
 ****************************************************************************************
 */
void ConsoleScan(void);

/**
 ****************************************************************************************
 * @brief Prints Console menu in Idle state.
 ****************************************************************************************
 */
void ConsoleIdle(void);

/**
 ****************************************************************************************
 * @brief Prints List of discovered devices.
 ****************************************************************************************
 */
void ConsolePrintScanList(void);

/**
 ****************************************************************************************
 * @brief Prints Console Menu in connected state.
 *
 * @param[in] full   If true, prints peers attributes values.
 ****************************************************************************************
 */
void ConsoleConnected(int full);

/**
 ****************************************************************************************
 * @brief Console Thread main loop.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
VOID ConsoleProc(PVOID unused);

/**
 ****************************************************************************************
 * @brief Prints connected devices list
 ****************************************************************************************
 */
void ConsoleQues(void);

/**
 ****************************************************************************************
 * @brief prints message "maximum connections reached"
 ****************************************************************************************
 */
void print_err_con_num(void);

/**
 ****************************************************************************************
 * @brief Prints Tabs.
 *
 *  @param[in] size  Size of string.
 *  @param[in] mode  Mode of print.
 ****************************************************************************************
 */
void printtabs(int size, int mode);

#endif //CONSOLE_H_
