/**
 ****************************************************************************************
 *
 * @file app_gattc_task.c
 *
 * @brief Generic Attribute Profile Service Application Task
 *
 * Copyright (C) 2018-2025 Renesas Electronics Corporation and/or its affiliates.
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

#include "rwip_config.h"        // SW Configuration

#if (BLE_APP_PRESENT)

#if (BLE_GATT_CLIENT)

#include <stdint.h>
#include "gatt_client_task.h"
#include "gatt_client.h"
#include "app_gattc.h"
#include "app_gattc_task.h"
#include "app_task.h"
#include "app_entry_point.h"
#include "app.h"
#include "user_profiles_config.h"
#include "user_callback_config.h"

/**
 ****************************************************************************************
 * @brief Handles GATT Client enable response message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_gattc_enable_handler(const ke_msg_id_t msgid,
                                    const struct gatt_client_enable_rsp *param,
                                    const ke_task_id_t dest_id,
                                    const ke_task_id_t src_id)
{
    CALLBACK_ARGS_3(user_app_gattc_cb.on_gattc_enable, KE_IDX_GET(src_id), param->status, &param->gatt);

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles GATT Client notification configuration write response message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_gattc_wr_cfg_ind_rsp_handler(const ke_msg_id_t msgid,
                                            const struct gatt_client_wr_cfg_ind_rsp *param,
                                            const ke_task_id_t dest_id,
                                            const ke_task_id_t src_id)
{
    CALLBACK_ARGS_2(user_app_gattc_cb.on_gattc_wr_cfg_ind_rsp, KE_IDX_GET(src_id), param->status);

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles GATT Client notification configuration read response message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_gattc_rd_cfg_ind_rsp_handler(const ke_msg_id_t msgid,
                                            const struct gatt_client_rd_cfg_ind_rsp *param,
                                            const ke_task_id_t dest_id,
                                            const ke_task_id_t src_id)
{
    bool read_val = (param->cfg_val == PRF_CLI_START_IND) ? true : false;

    CALLBACK_ARGS_3(user_app_gattc_cb.on_gattc_rd_cfg_ind_rsp, KE_IDX_GET(src_id), param->status, read_val);

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles GATT Clinet indication of the Service changed characteristic message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_gattc_svc_changed_ind_handler(const ke_msg_id_t msgid,
                                             struct gatt_client_svc_changed_ind *param,
                                             const ke_task_id_t dest_id,
                                             const ke_task_id_t src_id)
{
    CALLBACK_ARGS_2(user_app_gattc_cb.on_gattc_svc_changed_ind, KE_IDX_GET(src_id), &(param->val));
    return KE_MSG_CONSUMED;
}

/*
 * LOCAL VARIABLES DEFINITION
 ****************************************************************************************
 */

static const struct ke_msg_handler app_gattc_process_handlers[] =
{
    {GATT_CLIENT_ENABLE_RSP,        (ke_msg_func_t)app_gattc_enable_handler},
    {GATT_CLIENT_WR_CFG_IND_RSP,    (ke_msg_func_t)app_gattc_wr_cfg_ind_rsp_handler},
    {GATT_CLIENT_RD_CFG_IND_RSP,    (ke_msg_func_t)app_gattc_rd_cfg_ind_rsp_handler},
    {GATT_CLIENT_SVC_CHANGED_IND,   (ke_msg_func_t)app_gattc_svc_changed_ind_handler},
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_gattc_process_handler(ke_msg_id_t const msgid,
                                                      void const *param,
                                                      ke_task_id_t const dest_id,
                                                      ke_task_id_t const src_id,
                                                      enum ke_msg_status_tag *msg_ret)
{
    return (app_std_process_event(msgid, param, src_id, dest_id, msg_ret, app_gattc_process_handlers,
                                  sizeof(app_gattc_process_handlers) / sizeof(struct ke_msg_handler)));
}

#endif // (BLE_GATT_CLIENT)

#endif // BLE_APP_PRESENT

/// @} APP
