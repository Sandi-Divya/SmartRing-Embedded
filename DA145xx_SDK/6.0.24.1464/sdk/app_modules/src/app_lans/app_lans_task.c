/**
 ****************************************************************************************
 *
 * @file app_lans_task.c
 *
 *
 * @brief Location and Navigation Profile Server Application task implementation.
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

#if (BLE_LN_SENSOR)

#include "lans_task.h"
#include "app_lans.h"
#include "app_lans_task.h"
#include "app_task.h"                  // Application Task API
#include "app_entry_point.h"
#include "app.h"
#include "user_callback_config.h"

/**
 ****************************************************************************************
 * @brief Message handler for the LAN Server Application message (service enable).
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int lans_enable_rsp_handler(ke_msg_id_t const msgid,
                                       struct lans_enable_rsp const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    return (KE_MSG_CONSUMED);
}

/*
 ****************************************************************************************
 * @brief Message handler for the LAN Application sent loc speed confirmation.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int lans_ntf_loc_speed_rsp_handler(ke_msg_id_t const msgid,
                                       struct lans_ntf_loc_speed_rsp const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_lans_cb.on_lans_ntf_loc_speed, KE_IDX_GET(src_id), param->status);
    return (KE_MSG_CONSUMED);
}

/*
 ****************************************************************************************
 * @brief Message handler for the LAN Application sent nav confirmation.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int lans_ntf_navigation_rsp_handler(ke_msg_id_t const msgid,
                                       struct lans_ntf_navigation_rsp const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_lans_cb.on_lans_ntf_navigation, KE_IDX_GET(src_id), param->status);
    return (KE_MSG_CONSUMED);
}

/*
 ****************************************************************************************
 * @brief Message handler for the LAN Application sent pos q confirmation.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int lans_upd_pos_q_rsp_handler(ke_msg_id_t const msgid,
                                       struct lans_upd_pos_q_rsp const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_lans_cb.on_lans_upd_pos_q, KE_IDX_GET(src_id), param->status);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Message handler for the LAN Application CCC descriptor reconfiguration.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int lans_cfg_indntf_ind_handler(ke_msg_id_t const msgid,
                                       struct lans_cfg_ntfind_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    CALLBACK_ARGS_3(user_app_lans_cb.on_lans_ind_cfg_ind, KE_IDX_GET(src_id), param->char_code, param->ntf_cfg);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Message handler for the LAN Application control point indication
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int lans_ln_ctnl_pt_ind_handler(ke_msg_id_t const msgid,
                                       struct lans_ln_ctnl_pt_req_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{ 
    CALLBACK_ARGS_2(user_app_lans_cb.on_lans_ln_ctnl_pt_req_ind, KE_IDX_GET(src_id), param);
    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

const struct ke_msg_handler app_lans_process_handlers[]=
{
    {LANS_ENABLE_RSP,                   (ke_msg_func_t)lans_enable_rsp_handler},
    {LANS_NTF_LOC_SPEED_RSP,            (ke_msg_func_t)lans_ntf_loc_speed_rsp_handler},
    {LANS_NTF_NAVIGATION_RSP,           (ke_msg_func_t)lans_ntf_navigation_rsp_handler},
    {LANS_UPD_POS_Q_RSP,                (ke_msg_func_t)lans_upd_pos_q_rsp_handler},
    {LANS_CFG_NTFIND_IND,               (ke_msg_func_t)lans_cfg_indntf_ind_handler},
    {LANS_LN_CTNL_PT_REQ_IND,           (ke_msg_func_t)lans_ln_ctnl_pt_ind_handler},
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_lans_process_handler(ke_msg_id_t const msgid,
                                                       void const *param,
                                                       ke_task_id_t const dest_id,
                                                       ke_task_id_t const src_id,
                                                       enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret, app_lans_process_handlers,
                                         sizeof(app_lans_process_handlers) / sizeof(struct ke_msg_handler));
}

#endif // BLE_LN_SENSOR

#endif //(BLE_APP_PRESENT)

/// @} APPTASK
