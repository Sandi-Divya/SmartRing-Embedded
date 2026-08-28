/**
 ****************************************************************************************
 *
 * @file app_tips_task.c
 *
 *
 * @brief Time Profile Server Application task implementation.
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
 * @addtogroup APPTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"               // SW configuration

#if (BLE_APP_PRESENT)

#if (BLE_TIP_SERVER)

#include "tips_task.h"
#include "app_tips.h"
#include "app_tips_task.h"
#include "app_task.h"                  // Application Task API
#include "app_entry_point.h"
#include "app.h"
#include "user_callback_config.h"

/**
 ****************************************************************************************
 * @brief Message handler for the TIP Server Application message (service enable).
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int tips_enable_rsp_handler(ke_msg_id_t const msgid,
                                       struct tips_enable_rsp const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    return (KE_MSG_CONSUMED);
}

/*
 ****************************************************************************************
 * @brief Message handler for the TIP Application send update current time response.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int tips_upd_curr_time_rsp_handler(ke_msg_id_t const msgid,
                                       struct tips_upd_curr_time_rsp const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_tips_cb.on_tips_upd_curr_time_rsp, KE_IDX_GET(src_id), param->status);
    return (KE_MSG_CONSUMED);
}


/*
 ****************************************************************************************
 * @brief Message handler for the TIP Application sent a requested read.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int tips_rd_req_ind_handler(ke_msg_id_t const msgid,
                                       struct tips_rd_req_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_tips_cb.on_tips_rd_req_ind, KE_IDX_GET(src_id), param->char_code);
    return (KE_MSG_CONSUMED);
}

/*
 ****************************************************************************************
 * @brief Message handler for the TIP Application sent modification of current time characteristic.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int tips_current_time_ccc_ind_handler(ke_msg_id_t const msgid,
                                       struct tips_current_time_ccc_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_tips_cb.on_tips_current_time_ccc_ind, KE_IDX_GET(src_id), param->cfg_val);
    return (KE_MSG_CONSUMED);
}

/*
 ****************************************************************************************
 * @brief Message handler for the TIP Application sent modification of time update ctln pt value.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int tips_time_upd_ctnl_pt_ind_handler(ke_msg_id_t const msgid,
                                       struct tips_time_upd_ctnl_pt_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_tips_cb.on_tips_time_upd_ctnl_pt_ind, KE_IDX_GET(src_id), param->value);
    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

const struct ke_msg_handler app_tips_process_handlers[]=
{
    {TIPS_ENABLE_RSP,                   (ke_msg_func_t)tips_enable_rsp_handler},
    {TIPS_UPD_CURR_TIME_RSP,            (ke_msg_func_t)tips_upd_curr_time_rsp_handler},
    {TIPS_RD_REQ_IND,                   (ke_msg_func_t)tips_rd_req_ind_handler},
    {TIPS_CURRENT_TIME_CCC_IND,         (ke_msg_func_t)tips_current_time_ccc_ind_handler},
    {TIPS_TIME_UPD_CTNL_PT_IND,         (ke_msg_func_t)tips_time_upd_ctnl_pt_ind_handler},
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_tips_process_handler(ke_msg_id_t const msgid,
                                                       void const *param,
                                                       ke_task_id_t const dest_id,
                                                       ke_task_id_t const src_id,
                                                       enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret, app_tips_process_handlers,
                                         sizeof(app_tips_process_handlers) / sizeof(struct ke_msg_handler));
}

#endif // (BLE_TIP_SERVER)

#endif //(BLE_APP_PRESENT)

/// @} APPTASK
