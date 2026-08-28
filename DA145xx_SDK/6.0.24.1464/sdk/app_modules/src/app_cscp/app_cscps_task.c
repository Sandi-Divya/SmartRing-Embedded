/**
 ****************************************************************************************
 *
 * @file app_cscps_task.c
 *
 * @brief Cycling Speed and Cadence Profile Server Application task implementation.
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

#if (BLE_CSC_SENSOR)
#include "cscps_task.h"
#include "app_cscps.h"
#include "app_cscps_task.h"
#include "app_task.h"                  // Application Task API
#include "app_entry_point.h"
#include "app.h"
#include "user_callback_config.h"

/**
 ****************************************************************************************
 * @brief Message handler for CSCPS Application message (service enable).
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int cscps_enable_cfm_handler(ke_msg_id_t const msgid,
                                   struct cscps_enable_rsp const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_cscps_cb.on_cscps_enable_rsp, param->conidx, param->status);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Message handler for the CSCPS Application measurement send confirmation.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int cscps_meas_cfm_handler(ke_msg_id_t const msgid,
                                  struct cscps_ntf_csc_meas_rsp const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_cscps_cb.on_cscps_meas_cfm, KE_IDX_GET(src_id), param->status);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Message handler for the CSCPS Application measurement interval change indication.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int cscps_sc_cntl_pt_req_ind_handler(ke_msg_id_t const msgid,
                                  struct cscps_sc_ctnl_pt_req_ind const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_cscps_cb.on_cscps_sc_cntl_pt_req_ind, param->conidx, param);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Message handler for the CSCPS Application config descriptor reconfiguration.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int cscps_cfg_ntfind_ind_handler(ke_msg_id_t const msgid,
                                       struct cscps_cfg_ntfind_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_cscps_cb.on_cscps_cfg_ntfind_ind, param->conidx, param);
    
    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

const struct ke_msg_handler app_cscps_process_handlers[]=
{
    {CSCPS_ENABLE_RSP,           (ke_msg_func_t)cscps_enable_cfm_handler},
    {CSCPS_NTF_CSC_MEAS_RSP,     (ke_msg_func_t)cscps_meas_cfm_handler},
    {CSCPS_SC_CTNL_PT_REQ_IND,   (ke_msg_func_t)cscps_sc_cntl_pt_req_ind_handler},
    {CSCPS_CFG_NTFIND_IND,       (ke_msg_func_t)cscps_cfg_ntfind_ind_handler},
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_cscps_process_handler(ke_msg_id_t const msgid,
                                                     void const *param,
                                                     ke_task_id_t const dest_id,
                                                     ke_task_id_t const src_id,
                                                     enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret, app_cscps_process_handlers,
                                 sizeof(app_cscps_process_handlers) / sizeof(struct ke_msg_handler));
}

#endif //BLE_CSC_SENSOR

#endif //(BLE_APP_PRESENT)

/// @} APPTASK
