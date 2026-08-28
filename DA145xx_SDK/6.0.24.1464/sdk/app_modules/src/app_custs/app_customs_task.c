/**
 ****************************************************************************************
 *
 * @file app_customs_task.c
 *
 * @brief Custom Service application Task Implementation.
 *
 * Copyright (C) 2012-2025 Renesas Electronics Corporation and/or its affiliates.
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

#if (BLE_CUSTOM_SERVER)
#include "app_customs_task.h"
#include "custs1_task.h"
#include "custs2_task.h"
#include "app_task.h"           // Application Task API
#include "app_entry_point.h"
#include "app.h"

#if (BLE_CUSTOM1_SERVER)
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if 0
/**
 ****************************************************************************************
 * @brief Handles CUSTOM 1 Server profile database creation confirmation.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_custs1_create_db_cfm_handler(ke_msg_id_t const msgid,
                                            struct custs1_create_db_cfm const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == APP_DB_INIT)
    {
        // Inform the Application Manager
        struct app_module_init_cmp_evt *cfm = KE_MSG_ALLOC(APP_MODULE_INIT_CMP_EVT,
                                                           TASK_APP, TASK_APP,
                                                           app_module_init_cmp_evt);

        cfm->status = param->status;

        KE_MSG_SEND(cfm);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles disable indication from the CUSTOM 1 Server profile.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_custs1_disable_ind_handler(ke_msg_id_t const msgid,
                                          struct custs1_disable_ind const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

static const struct ke_msg_handler app_custs1_process_handlers[] =
{
    {CUSTS1_CREATE_DB_CFM,                 (ke_msg_func_t)app_custs1_create_db_cfm_handler},
    {CUSTS1_DISABLE_IND,                   (ke_msg_func_t)app_custs1_disable_ind_handler},
};
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_custs1_process_handler(ke_msg_id_t const msgid,
                                                       void const *param,
                                                       ke_task_id_t const dest_id,
                                                       ke_task_id_t const src_id,
                                                       enum ke_msg_status_tag *msg_ret)
{
#if 0
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret, app_custs1_process_handlers,
                                         sizeof(app_custs1_process_handlers) / sizeof(struct ke_msg_handler));
#else
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret, NULL, 0);
#endif
}
#endif //BLE_CUSTOM1_SERVER

#if (BLE_CUSTOM2_SERVER)
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if 0
/**
 ****************************************************************************************
 * @brief Handles CUSTOM 2 Server profile database creation confirmation.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_custs2_create_db_cfm_handler(ke_msg_id_t const msgid,
                                            struct custs2_create_db_cfm const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == APP_DB_INIT)
    {
        // Inform the Application Manager
        struct app_module_init_cmp_evt *cfm = KE_MSG_ALLOC(APP_MODULE_INIT_CMP_EVT,
                                                           TASK_APP, TASK_APP,
                                                           app_module_init_cmp_evt);

        cfm->status = param->status;

        KE_MSG_SEND(cfm);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles disable indication from the CUSTOM 2 Server profile.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_custs2_disable_ind_handler(ke_msg_id_t const msgid,
                                          struct custs2_disable_ind const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

static const struct ke_msg_handler app_custs2_process_handlers[] =
{
    {CUSTS2_CREATE_DB_CFM,                 (ke_msg_func_t)app_custs2_create_db_cfm_handler},
    {CUSTS2_DISABLE_IND,                   (ke_msg_func_t)app_custs2_disable_ind_handler},
};
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_custs2_process_handler(ke_msg_id_t const msgid,
                                                       void const *param,
                                                       ke_task_id_t const dest_id,
                                                       ke_task_id_t const src_id,
                                                       enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret, NULL, 0);
}
#endif // BLE_CUSTOM2_SERVER

#endif // BLE_CUSTOM_SERVER

#endif // BLE_APP_PRESENT

/// @} APPTASK
