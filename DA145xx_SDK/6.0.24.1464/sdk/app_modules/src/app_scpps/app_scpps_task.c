/**
 ****************************************************************************************
 *
 * @file app_scpps_task.c
 *
 * @brief Scan Parameters Profile Server Application task implementation.
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

#if (BLE_SP_SERVER)
#include "scpps_task.h"
#include "app_scpps.h"
#include "app_scpps_task.h"
#include "app_task.h"                  // Application Task API
#include "app_entry_point.h"
#include "app.h"
#include "user_callback_config.h"

/**
 ****************************************************************************************
 * @brief Message handler for SCPP Application message (service enable).
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int scpps_enable_cfm_handler(ke_msg_id_t const msgid,
                                   struct scpps_enable_rsp const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    CALLBACK_ARGS_1(user_app_scpps_cb.on_scpps_enable_rsp, param);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Message handler for the SCPP Application notify indication.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id  ID of the sending task instance
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
static int scpps_ntf_immediate_req_ind_handler(ke_msg_id_t const msgid,
                                  const struct scpps_scan_refresh_ntf_cfg_ind *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(src_id);

    CALLBACK_ARGS_2(user_app_scpps_cb.on_scpps_scan_refresh_ntf_cfg_ind, conidx, param);

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

const struct ke_msg_handler app_scpps_process_handlers[]=
{
    {SCPPS_ENABLE_RSP,               (ke_msg_func_t)scpps_enable_cfm_handler},
    {SCPPS_SCAN_REFRESH_NTF_CFG_IND, (ke_msg_func_t)scpps_ntf_immediate_req_ind_handler},
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_scpps_process_handler(ke_msg_id_t const msgid,
                                                     void const *param,
                                                     ke_task_id_t const dest_id,
                                                     ke_task_id_t const src_id,
                                                     enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret, app_scpps_process_handlers,
                                 sizeof(app_scpps_process_handlers) / sizeof(struct ke_msg_handler));
}

#endif //BLE_SP_SERVER

#endif //(BLE_APP_PRESENT)

/// @} APPTASK
