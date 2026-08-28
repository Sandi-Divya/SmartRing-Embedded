/**
 ****************************************************************************************
 *
 * @file app_lecb_task.c
 *
 * @brief Application LE Credit-Based Channels Task implementation
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
 * @addtogroup APPLECBTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_lecb_task.h"
#include "user_callback_config.h"
#include "gapc_task.h"
#include "l2cc_task.h"
#include "l2cc_pdu.h"
#include "ke_msg.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

int app_lecb_disconnect_ind_handler(ke_msg_id_t msgid,
                                     struct gapc_lecb_disconnect_ind *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_lecb_callbacks.app_lecb_disconnect_ind, KE_IDX_GET(src_id), param)

    return (KE_MSG_CONSUMED);
}

int app_lecb_connect_req_ind_handler(ke_msg_id_t msgid,
                                      struct gapc_lecb_connect_req_ind *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    uint16_t status = L2C_CB_CON_SUCCESS;

    CALLBACK_ARGS_3(user_app_lecb_callbacks.app_lecb_connect_req_ind, KE_IDX_GET(src_id), param, &status)

    struct gapc_lecb_connect_cfm *cfm = KE_MSG_ALLOC(GAPC_LECB_CONNECT_CFM,
                                                   KE_BUILD_ID(TASK_GAPC, KE_IDX_GET(src_id)),
                                                   TASK_APP,
                                                   gapc_lecb_connect_cfm);

    cfm->le_psm = param->le_psm;
    cfm->status = status;

    // Send message
    KE_MSG_SEND(cfm);
    return (KE_MSG_CONSUMED);
}

int app_lecb_connect_ind_handler(ke_msg_id_t msgid,
                                 struct gapc_lecb_connect_ind *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_lecb_callbacks.app_lecb_connect_ind, KE_IDX_GET(src_id), param)

    return (KE_MSG_CONSUMED);
}

int app_lecb_recv_data_ind_handler(ke_msg_id_t msgid,
                                   struct l2cc_lecnx_data_recv_ind *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_lecb_callbacks.app_lecb_data_recv_ind, KE_IDX_GET(src_id), param)

    return (KE_MSG_CONSUMED);
}

int app_lecb_add_ind_handler(ke_msg_id_t msgid,
                             struct gapc_lecb_add_ind *param,
                             ke_task_id_t const dest_id,
                             ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_lecb_callbacks.app_lecb_add_ind, KE_IDX_GET(src_id), param)

    return (KE_MSG_CONSUMED);
}

int app_lecb_pdu_send_rsp_handler(ke_msg_id_t msgid,
                                  struct l2cc_data_send_rsp *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_lecb_callbacks.app_lecb_data_send_rsp, KE_IDX_GET(src_id), param)

    return (KE_MSG_CONSUMED);
}
/// @} APPLECBTASK
