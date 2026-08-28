/**
 ****************************************************************************************
 *
 * @file app_proxr_task.c
 *
 * @brief Proximity Reporter application Task Implementation.
 *
 * Copyright (C) 2012-2023 Renesas Electronics Corporation and/or its affiliates.
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

#include "rwble_config.h"              // SW configuration

#if (BLE_APP_PRESENT)

#if (BLE_PROX_REPORTER)

#include "app_proxr.h"
#include "app_proxr_task.h"
#include "arch_api.h"
#include "app_entry_point.h"
#include "app.h"
#include "user_callback_config.h"

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles proximity alert timer.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_proxr_timer_handler(ke_msg_id_t const msgid,
                                   void const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    if (alert_state.blink_toggle)
    {
        GPIO_SetInactive(alert_state.port, alert_state.pin);
        alert_state.blink_toggle = 0;
    }
    else
    {
        GPIO_SetActive(alert_state.port, alert_state.pin);
        alert_state.blink_toggle = 1;
    }

    KE_TIMER_SET(APP_PROXR_TIMER, dest_id, alert_state.blink_timeout);

    return (KE_MSG_CONSUMED);
}

static int proxr_alert_ind_handler(ke_msg_id_t const msgid,
                                   struct proxr_alert_ind const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    CALLBACK_ARGS_3(user_app_proxr_cb.on_proxr_alert_ind, param->conidx, param->alert_lvl, param->char_code)

    return (KE_MSG_CONSUMED);
}

/*
 * LOCAL VARIABLES DEFINITION
 ****************************************************************************************
 */

static const struct ke_msg_handler app_proxr_process_handlers[] =
{
    {APP_PROXR_TIMER,                       (ke_msg_func_t)app_proxr_timer_handler},
    {PROXR_ALERT_IND,                       (ke_msg_func_t)proxr_alert_ind_handler},
};

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_proxr_process_handler(ke_msg_id_t const msgid,
                                                      void const *param,
                                                      ke_task_id_t const dest_id,
                                                      ke_task_id_t const src_id,
                                                      enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret,
                                 app_proxr_process_handlers,
                                 sizeof(app_proxr_process_handlers) / sizeof(struct ke_msg_handler));
}

#endif //BLE_PROX_REPORTER

#endif //(BLE_APP_PRESENT)

/// @} APPTASK
