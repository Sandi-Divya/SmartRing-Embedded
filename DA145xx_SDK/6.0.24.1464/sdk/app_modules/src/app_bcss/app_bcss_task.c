/**
 ****************************************************************************************
 *
 * @file app_bcss_task.c
 *
 * @brief Body Composition Service application task.
 *
 * Copyright (C) 2017-2025 Renesas Electronics Corporation and/or its affiliates.
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

#include "rwip_config.h"

#if (BLE_APP_PRESENT)

#if (BLE_BCS_SERVER)

#include "bcss_task.h"
#include "app_entry_point.h"
#include "user_callback_config.h"

/*
 * LOCAL VARIABLES DEFINITION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Message handler for the BCS Server Application message (service enable).
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int bcss_enable_rsp_handler(ke_msg_id_t const msgid,
                                   struct bcss_enable_rsp const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    CALLBACK_ARGS_3(user_app_bcss_cb.on_bcss_enable_rsp, KE_IDX_GET(src_id),
                    param->status, param->init_cfg_ind);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Message handler for the BCS measurement send response.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int bcss_meas_val_ind_rsp_handler(ke_msg_id_t const msgid,
                                         struct bcss_meas_val_ind_rsp const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_bcss_cb.on_bcss_meas_val_ind_rsp,
                    KE_IDX_GET(src_id), param->status);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Message handler for the BCS CCC descriptor configuration change.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int bcss_meas_val_ind_cfg_ind_handler(ke_msg_id_t const msgid,
                                             struct bcss_meas_val_ind_cfg_ind *param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
    bool enabled = param->ind_cfg ? true : false;

    CALLBACK_ARGS_2(user_app_bcss_cb.on_bcss_ind_cfg_ind,
                    KE_IDX_GET(src_id), enabled);

    return (KE_MSG_CONSUMED);
}

static const struct ke_msg_handler app_bcss_process_handlers[] =
{
    {BCSS_ENABLE_RSP,               (ke_msg_func_t)bcss_enable_rsp_handler},
    {BCSS_MEAS_VAL_IND_RSP,         (ke_msg_func_t)bcss_meas_val_ind_rsp_handler},
    {BCSS_MEAS_VAL_IND_CFG_IND,     (ke_msg_func_t)bcss_meas_val_ind_cfg_ind_handler},
};

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_bcss_process_handler(ke_msg_id_t const msgid,
                                                     void const *param,
                                                     ke_task_id_t const dest_id,
                                                     ke_task_id_t const src_id,
                                                     enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param,src_id, dest_id, msg_ret, app_bcss_process_handlers,
                                    sizeof(app_bcss_process_handlers) / sizeof(struct ke_msg_handler));
}

#endif // (BLE_BCS_SERVER)

#endif // (BLE_APP_PRESENT)

/// @} APPTASK
