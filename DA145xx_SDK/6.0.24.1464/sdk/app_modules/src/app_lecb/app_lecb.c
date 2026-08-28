/**
 ****************************************************************************************
 *
 * @file app_lecb.c
 *
 * @brief LE Credit-Based channels application.
 *
 * Copyright (C) 2023-2025 Renesas Electronics Corporation and/or its affiliates.
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

/**
 ****************************************************************************************
 * @addtogroup APP_LECB
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_lecb.h"
#include "gapc_task.h"
#include "l2cc_task.h"
#include "ke_msg.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_lecb_connect_channel(uint8_t conidx, uint16_t le_psm, uint16_t cid, uint16_t credit)
{
    struct gapc_lecb_connect_cmd *cmd = KE_MSG_ALLOC(GAPC_LECB_CONNECT_CMD,
                                                     KE_BUILD_ID(TASK_GAPC, conidx),
                                                     TASK_APP,
                                                     gapc_lecb_connect_cmd);

    cmd->operation = GAPC_LE_CB_CONNECTION;
    cmd->le_psm = le_psm;
    cmd->cid = cid;
    cmd->credit = credit;
    KE_MSG_SEND(cmd);
}

void app_lecb_create_channel(uint8_t conidx, uint16_t sec_level, uint16_t le_psm, uint16_t cid, uint16_t initial_credit)
{
    struct gapc_lecb_create_cmd *cmd = KE_MSG_ALLOC(GAPC_LECB_CREATE_CMD,
                                                    KE_BUILD_ID(TASK_GAPC, conidx),
                                                    TASK_APP,
                                                    gapc_lecb_create_cmd);

    uint16_t perm = 0;

    if (sec_level == GAP_NO_SEC) perm = (PERM_RIGHT_DISABLE << PERM_POS_SVC_AUTH) | (0 << PERM_POS_SVC_EKS);
    else if (sec_level == GAP_SEC1_NOAUTH_PAIR_ENC) perm = (PERM_RIGHT_UNAUTH << PERM_POS_SVC_AUTH) | (1 << PERM_POS_SVC_EKS);
    else if (sec_level == GAP_SEC1_AUTH_PAIR_ENC) perm = (PERM_RIGHT_AUTH << PERM_POS_SVC_AUTH) | (1 << PERM_POS_SVC_EKS);
    else if (sec_level == GAP_SEC1_SEC_PAIR_ENC) perm = (PERM_RIGHT_SECURE << PERM_POS_SVC_AUTH) | (1 << PERM_POS_SVC_EKS);

    cmd->operation = GAPC_LE_CB_CREATE;
    cmd->sec_lvl = perm;
    cmd->le_psm = le_psm;
    cmd->cid = cid;
    cmd->intial_credit = initial_credit;
    KE_MSG_SEND(cmd);
}

void app_lecb_destroy_channel(uint8_t conidx, uint16_t le_psm)
{
    struct gapc_lecb_destroy_cmd *cmd = KE_MSG_ALLOC(GAPC_LECB_DESTROY_CMD,
                                                     KE_BUILD_ID(TASK_GAPC, conidx),
                                                     TASK_APP,
                                                     gapc_lecb_destroy_cmd);

    cmd->operation = GAPC_LE_CB_DESTROY;
    cmd->le_psm = le_psm;
    KE_MSG_SEND(cmd);
}

void app_lecb_disconnect_channel(uint8_t conidx, uint16_t le_psm)
{
    struct gapc_lecb_disconnect_cmd *cmd = KE_MSG_ALLOC(GAPC_LECB_DISCONNECT_CMD,
                                                        KE_BUILD_ID(TASK_GAPC, conidx),
                                                        TASK_APP,
                                                        gapc_lecb_disconnect_cmd);

    cmd->operation = GAPC_LE_CB_DISCONNECTION;
    cmd->le_psm = le_psm;
    KE_MSG_SEND(cmd);
}

void app_lecb_add_credits(uint8_t conidx, uint16_t le_psm, uint16_t credit)
{
    struct gapc_lecb_add_cmd *cmd = KE_MSG_ALLOC(GAPC_LECB_ADD_CMD,
                                                 KE_BUILD_ID(TASK_GAPC, conidx),
                                                 TASK_APP,
                                                 gapc_lecb_add_cmd);

    cmd->operation = GAPC_LE_CB_ADDITION;
    cmd->le_psm = le_psm;
    cmd->credit = credit;
    KE_MSG_SEND(cmd);
}

void app_lecb_send_sdu(uint8_t conidx, uint16_t cid, uint16_t length, const void *data)
{
    struct l2cc_pdu_send_req *pkt = KE_MSG_ALLOC_DYN(L2CC_PDU_SEND_REQ,
                                                     KE_BUILD_ID(TASK_L2CC, conidx),
                                                     TASK_APP,
                                                     l2cc_pdu_send_req,
                                                     length);

    pkt->offset = 0;
    pkt->pdu.payld_len = 0;
    pkt->pdu.chan_id   = cid;
    pkt->pdu.data.send_lecb_data_req.code = 0;
    pkt->pdu.data.send_lecb_data_req.sdu_data_len = length;
    memcpy(&(pkt->pdu.data.send_lecb_data_req.sdu_data), data, length);
    KE_MSG_SEND(pkt);
}

/// @} APP_LECB
