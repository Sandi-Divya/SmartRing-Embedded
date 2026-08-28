/**
 ****************************************************************************************
 *
 * @file app_udss_task.c
 *
 * @brief User Data Service application task.
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

#if (BLE_UDS_SERVER)

#include "udss_task.h"
#include "app_entry_point.h"
#include "user_callback_config.h"

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles UDS Characteristic value request message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udss_char_val_req_handler(ke_msg_id_t const msgid,
                                     struct udss_char_val_req const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_udss_cb.on_udss_char_val_req, KE_IDX_GET(src_id),
                    param->char_code)

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles UDS Control Point request message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udss_ucp_req_ind_handler(ke_msg_id_t const msgid,
                                    struct udss_ucp_req_ind const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_udss_cb.on_udss_cntl_point_write_ind, KE_IDX_GET(src_id),
                    &param->ucp_req)

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles UDS Characteristic value indication message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udss_char_val_ind_handler(ke_msg_id_t const msgid,
                                     struct udss_char_val const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    CALLBACK_ARGS_4(user_app_udss_cb.on_udss_char_val_ind, KE_IDX_GET(src_id),
                    param->char_code, param->length, param->value);

    return (KE_MSG_CONSUMED);
}

/*
 * LOCAL VARIABLES DEFINITION
 ****************************************************************************************
 */

static const struct ke_msg_handler app_udss_process_handlers[] =
{
    {UDSS_CHAR_VAL_REQ,         (ke_msg_func_t)udss_char_val_req_handler},
    {UDSS_UCP_REQ_IND,          (ke_msg_func_t)udss_ucp_req_ind_handler},
    {UDSS_SET_CHAR_VAL_IND,     (ke_msg_func_t)udss_char_val_ind_handler},
};

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_udss_process_handler(ke_msg_id_t const msgid,
                                                     void const *param,
                                                     ke_task_id_t const dest_id,
                                                     ke_task_id_t const src_id,
                                                     enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param,src_id, dest_id, msg_ret, app_udss_process_handlers,
                                 sizeof(app_udss_process_handlers) / sizeof(struct ke_msg_handler));
}

#endif // (BLE_UDS_SERVER)

#endif // (BLE_APP_PRESENT)

/// @} APPTASK
