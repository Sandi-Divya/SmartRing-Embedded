/**
 ****************************************************************************************
 *
 * @file ble_msg.h
 *
 * @brief Header file for reception of ble messages sent from DA14585 embedded application over UART interface.
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

#ifndef BLE_MSG_H_
#define BLE_MSG_H_

#include "rwble_config.h"

typedef struct {
    unsigned short bType;
    unsigned short bDstid;
    unsigned short bSrcid;
    unsigned short bLength;
} ble_hdr;

typedef struct {
    unsigned short bType;
    unsigned short bDstid;
    unsigned short bSrcid;
    unsigned short bLength;
    unsigned char  bData[1];
} ble_msg;

/*
 ****************************************************************************************
 * @brief Send message to UART iface.
 *
 * @param[in] msg   pointer to message.
 ****************************************************************************************
*/
void BleSendMsg(void *msg);

/*
 ****************************************************************************************
 * @brief Allocate memory for ble message.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @param[in] param_len Parameters length.
 *
 * @return pointer to allocated message
 ****************************************************************************************
*/
void *BleMsgAlloc(unsigned short id, unsigned short dest_id,
                  unsigned short src_id, unsigned short param_len);

/*
 ****************************************************************************************
 * @brief Allocate memory for ble message.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @param[in] param_len Parameters length.
 * @param[in] length    Length for the data
 *
 * @return pointer to allocated message
 ****************************************************************************************
*/
void *BleMsgDynAlloc(unsigned short id, unsigned short dest_id,
                     unsigned short src_id, unsigned short param_len, unsigned short length);

/*
 ****************************************************************************************
 * @brief Handles ble by calling corresponding procedure.
 *
 * @param[in] blemsg    Pointer to received message.
 ****************************************************************************************
*/
void HandleBleMsg(ble_msg *blemsg);

/*
 ****************************************************************************************
 * @brief Receives ble message from UART iface.
 ****************************************************************************************
*/
void BleReceiveMsg(void);


#endif //BLE_MSG_H_
