/**
 ****************************************************************************************
 *
 * @file user_lecb.c
 *
 * @brief LE Credit-Based Channels project source code.
 *
 * Copyright (C) 2023-2024 Renesas Electronics Corporation and/or its affiliates.
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
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gattc_task.h"
#include "gap.h"
#include "app_easy_timer.h"
#include "user_lecb.h"
#include "co_bt.h"
#include "app_lecb.h"
#include "app_lecb_task.h"
#include "gapc_task.h"
#include "l2cc_pdu.h"
#include "l2cc_task.h"
#include "app_easy_security.h"
#include "app_security.h"
#include "arch_console.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef struct {
    uint8_t connection_id;
    uint16_t le_psm;
    uint16_t cid;
    uint16_t dest_credits;
    uint16_t src_credits;
    uint16_t max_SDU;
} user_lecb_connection_t;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

uint8_t app_connection_idx                      __SECTION_ZERO("retention_mem_area0");
timer_hnd app_param_update_request_timer_used   __SECTION_ZERO("retention_mem_area0");
user_lecb_connection_t user_lecb_conn           __SECTION_ZERO("retention_mem_area0");

const uint8_t test_string[] = "Data over Credit Based channel";

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

/**
 ****************************************************************************************
 * @brief Parameter update request timer callback function.
 ****************************************************************************************
*/
static void param_update_request_timer_cb()
{
    app_easy_gap_param_update_start(app_connection_idx);
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
}

void user_app_init(void)
{
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;

    default_app_on_init();
#if (BLE_APP_SEC)
    // Fetch bond data from the external memory
    app_easy_security_bdb_init();
#endif // (BLE_APP_SEC)
}

void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        app_connection_idx = connection_idx;

        arch_printf(" \r\n Create LE credit based channel");
        app_lecb_create_channel(connection_idx, user_security_conf.sec_req, USER_L2CAP_LEPSM, USER_L2CAP_CID, USER_LECB_CREDITS);

        // Check if the parameters of the established connection are the preferred ones.
        // If not then schedule a connection parameter update request.
        if ((param->con_interval < user_connection_param_conf.intv_min) ||
            (param->con_interval > user_connection_param_conf.intv_max) ||
            (param->con_latency != user_connection_param_conf.latency) ||
            (param->sup_to != user_connection_param_conf.time_out))
        {
            // Connection params are not these that we expect
            app_param_update_request_timer_used = app_easy_timer(APP_PARAM_UPDATE_REQUEST_TO, param_update_request_timer_cb);
        }
    }
    else
    {
        // No connection has been established, restart advertising
        default_advertise_operation();
    }

    default_app_on_connection(connection_idx, param);
}

void user_app_adv_undirect_complete(uint8_t status)
{
    // If advertising was canceled then update advertising data and start advertising again
    if (status == GAP_ERR_CANCELED)
    {
        default_advertise_operation();
    }
}

void user_app_disconnect(struct gapc_disconnect_ind const *param)
{
    // Cancel the parameter update request timer
    if (app_param_update_request_timer_used != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_param_update_request_timer_used);
        app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    }

    // Restart Advertising
    default_advertise_operation();
}

void user_app_lecb_disconnect_ind(uint8_t conidx,
                                  struct gapc_lecb_disconnect_ind const *param)
{
    arch_printf(" \r\n Disconnected from LE credit based channel, PSM: 0x%x, Reason: 0x%x", param->le_psm, param->reason);
}

void user_app_lecb_connect_req_ind(uint8_t conidx,
                                   struct gapc_lecb_connect_req_ind const *param, uint16_t *status)
{
    *status = L2C_CB_CON_SUCCESS;
}

void user_app_lecb_connect_ind(uint8_t conidx,
                               struct gapc_lecb_connect_ind const *param)
{
    user_lecb_conn.connection_id = conidx;
    user_lecb_conn.le_psm = param->le_psm;
    user_lecb_conn.dest_credits = param->dest_credit;
    user_lecb_conn.max_SDU = param->max_sdu;
    user_lecb_conn.cid = param->dest_cid;
    arch_printf(" \r\n Connected to LE credit based channel, PSM: 0x%x, Credit: 0x%x, Maximum SDU size: 0x%x, CID: 0x%x",
                                                                                param->le_psm, param->dest_credit, param->max_sdu, param->dest_cid);
}

void user_app_lecb_add_ind(uint8_t conidx,
                           struct gapc_lecb_add_ind const *param)
{
    uint16_t credits_added;
    
    if(user_lecb_conn.dest_credits < param->dest_credit) {
        credits_added = param->dest_credit - user_lecb_conn.dest_credits;
    } else {
        credits_added = param->dest_credit;
    }
    
    user_lecb_conn.src_credits += credits_added;
    user_lecb_conn.dest_credits = param->dest_credit;
    app_lecb_add_credits(conidx, user_lecb_conn.le_psm, credits_added);
    arch_printf(" \r\n Credits added");
}

void user_app_lecb_data_recv_ind(uint8_t conidx,
                                 struct l2cc_lecnx_data_recv_ind const *param)
{
    user_lecb_conn.src_credits = param->src_credit;
    arch_printf(" \r\n Received message: %d", param->data);
    app_lecb_send_sdu(conidx, user_lecb_conn.cid, sizeof(test_string), test_string);
    arch_printf(" \r\n Send message: %s", test_string);
}

void user_app_lecb_data_send_rsp(uint8_t conidx, struct l2cc_data_send_rsp const *param)
{
    switch (param->status)
    {
        case GAP_ERR_NO_ERROR:
        {
            arch_printf(" \r\n L2CC_PDU_SEND_RSP -> GAP_ERR_NO_ERROR");
        }
        break;

        case L2C_ERR_INSUFF_CREDIT:
        {
            arch_printf(" \r\n L2CC_PDU_SEND_RSP -> L2C_ERR_INSUFF_CREDIT");
        }
        break;

        case L2C_ERR_INVALID_MTU_EXCEED:
        {
            arch_printf(" \r\n L2CC_PDU_SEND_RSP -> L2C_ERR_INVALID_MTU_EXCEED");
        }
        break;
    }
}

void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    switch(msgid)
    {
        case GAPC_PARAM_UPDATED_IND:
        {
            // Cast the "param" pointer to the appropriate message structure
            struct gapc_param_updated_ind const *msg_param = (struct gapc_param_updated_ind const *)(param);

            // Check if updated Conn Params filled to preferred ones
            if ((msg_param->con_interval >= user_connection_param_conf.intv_min) &&
                (msg_param->con_interval <= user_connection_param_conf.intv_max) &&
                (msg_param->con_latency == user_connection_param_conf.latency) &&
                (msg_param->sup_to == user_connection_param_conf.time_out))
            {
            }
        } break;

        case GAPC_LECB_CONNECT_REQ_IND:
        {
            app_lecb_connect_req_ind_handler(msgid, (struct gapc_lecb_connect_req_ind*) param, dest_id, src_id);
            break;
        }

        case GAPC_LECB_CONNECT_IND:
        {
            app_lecb_connect_ind_handler(msgid, (struct gapc_lecb_connect_ind*) param, dest_id, src_id);
            break;
        };

        case L2CC_LECNX_DATA_RECV_IND:
        {
            app_lecb_recv_data_ind_handler(msgid, (struct l2cc_lecnx_data_recv_ind*) param, dest_id, src_id);
            break;
        }

        case L2CC_PDU_SEND_RSP:
        {
            app_lecb_pdu_send_rsp_handler(msgid, (struct l2cc_data_send_rsp*) param, dest_id, src_id);
            break;
        }

        case GAPC_LECB_ADD_IND:
        {
            app_lecb_add_ind_handler(msgid, (struct gapc_lecb_add_ind*) param, dest_id, src_id);
            break;
        }

        case GAPC_LECB_DISCONNECT_IND:
        {
            app_lecb_disconnect_ind_handler(msgid, (struct gapc_lecb_disconnect_ind*)param, dest_id, src_id);
            break;
        }

        case GATTC_EVENT_REQ_IND:
        {
            // Confirm unhandled indication to avoid GATT timeout
            struct gattc_event_ind const *ind = (struct gattc_event_ind const *) param;
            struct gattc_event_cfm *cfm = KE_MSG_ALLOC(GATTC_EVENT_CFM, src_id, dest_id, gattc_event_cfm);
            cfm->handle = ind->handle;
            KE_MSG_SEND(cfm);
        } break;

        default:
            break;
    }
}

/// @} APP
