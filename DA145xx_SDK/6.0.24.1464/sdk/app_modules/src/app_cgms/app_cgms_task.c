/**
 ****************************************************************************************
 *
 * @file app_cgms_task.c
 *
 * @brief Continuous Glucose Monitoring Profile Server Application task implementation.
 *
 * Copyright (C) 2022-2024 Renesas Electronics Corporation and/or its affiliates.
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

#if (BLE_CGM_SERVER)
#include "cgms_task.h"
#include "app_cgms.h"
#include "app_cgms_task.h"
#include "app_task.h"                  // Application Task API
#include "app_entry_point.h"
#include "app.h"
#include "user_callback_config.h"
#include "user_profiles_config.h"
#include "arch_console.h"


/**
 ****************************************************************************************
 * @brief Message handler for the CGM Server Application message (service enable).
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int cgms_enable_rsp_handler(ke_msg_id_t const msgid,
                                       struct cgms_enable_rsp const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
		
    CALLBACK_ARGS_2(user_app_cgms_cb.on_cgms_enable_rsp, KE_IDX_GET(src_id), param->status);
    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Message handler for the CGMS Application CCC descriptor reconfiguration.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int cgms_cfg_indntf_ind_handler(ke_msg_id_t const msgid,
                                       struct cgms_cfg_indntf_ind const *ind,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
   
    CALLBACK_ARGS_2(user_app_cgms_cb.on_cgms_cfg_indntf_ind, ind->conidx, ind->ntf_ind_cfg);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Message handler for the CGMS Application Status Send Request.
 * @param[in] msgid   Id of the message received
 * @param[in] ind     Pointer to the message struct
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int cgms_status_send_req_handler(ke_msg_id_t const msgid,
                                       struct cgms_status_send_req const *ind,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    CALLBACK_ARGS_1(user_app_cgms_cb.on_cgms_status_send_req, ind->conidx);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Message handler for the CGMS Application Status Send Request.
 * @param[in] msgid   Id of the message received
 * @param[in] ind     Pointer to the message struct
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int cgms_session_start_time_req_handler(ke_msg_id_t const msgid,
                                               struct cgms_session_start_time_req const *req,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id)
{
    uint8_t state  = ke_state_get(dest_id);

    if(state == CGMS_IDLE)
    {
        // Get the address of the environment
        struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);
        
        memcpy(&cgms_env[req->conidx].session_start_time, &req->session_start_time, sizeof(struct cgm_session_start_time));
    }
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Message handler for the CGMS RACP Request.
 * @param[in] msgid   Id of the message received
 * @param[in] ind     Pointer to the message struct
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int cgms_racp_req_rcv_ind_handler(ke_msg_id_t const msgid,
                                       struct cgms_racp_req_rcv_ind const *ind,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_cgms_cb.on_cgms_racp_req_rcv_ind, ind->conidx, &ind->racp_req);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Message handler for the CGMS Special Ops Control Point Request.
 * @param[in] msgid   Id of the message received
 * @param[in] ind     Pointer to the message struct
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int cgms_spec_ops_cp_req_rcv_ind_handler(ke_msg_id_t const msgid,
                                            struct cgms_spec_ops_cp_req_rcv_ind const *ind,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_cgms_cb.on_cgms_spec_ops_cp_req_rcv_ind, ind->conidx, &ind->spec_ops_cp_req);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Message handler for the CGMS Complete Event (after notification/indication sent).
 * @param[in] msgid   Id of the message received
 * @param[in] ind     Pointer to the message struct
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int cgms_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct cgms_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_cgms_cb.on_cgms_cmp_evt, param->request, param->status);
    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

const struct ke_msg_handler app_cgms_process_handlers[]=
{
    {CGMS_ENABLE_RSP,                  (ke_msg_func_t)cgms_enable_rsp_handler},
    
    {CGMS_CFG_INDNTF_IND,              (ke_msg_func_t)cgms_cfg_indntf_ind_handler},
    {CGMS_STATUS_SEND_REQ,             (ke_msg_func_t)cgms_status_send_req_handler},
    {CGMS_SESSION_START_TIME_REQ,      (ke_msg_func_t)cgms_session_start_time_req_handler},
    {CGMS_RACP_REQ_RCV_IND,            (ke_msg_func_t)cgms_racp_req_rcv_ind_handler},
    {CGMS_SPEC_OPS_CP_REQ_RCV_IND,     (ke_msg_func_t)cgms_spec_ops_cp_req_rcv_ind_handler},
    {CGMS_CMP_EVT,                     (ke_msg_func_t)cgms_cmp_evt_handler},
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_cgms_process_handler(ke_msg_id_t const msgid,
                                                       void const *param,
                                                       ke_task_id_t const dest_id,
                                                       ke_task_id_t const src_id,
                                                       enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret, app_cgms_process_handlers,
                                         sizeof(app_cgms_process_handlers) / sizeof(struct ke_msg_handler));
}

#endif //BLE_CGM_SERVER

#endif //(BLE_APP_PRESENT)

/// @} APPTASK
