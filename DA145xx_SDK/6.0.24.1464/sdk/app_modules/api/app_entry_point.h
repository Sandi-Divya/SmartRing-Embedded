/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Entry_Point Entry Point
 * @brief Application entry point API
 * @{
 *
 * @file app_entry_point.h
 *
 * @brief Application entry point header file.
 *
 * Copyright (C) 2015-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _APP_ENTRY_POINT_H_
#define _APP_ENTRY_POINT_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "ke_msg.h"
#include "ke_task.h"
#include "app_callback.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Helper macro to identify a task
#define KE_FIND_RELATED_TASK(task) ((ke_msg_id_t)((task) >> 10))

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Format of a catch rest event handler function
typedef void(*catch_rest_event_func_t)(ke_msg_id_t const msgid,
                                       void const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id);

struct app_entry_catch_rest
{
    catch_rest_event_func_t cb;
};

/// Process event response
enum process_event_response
{
    /// Handled
    PR_EVENT_HANDLED = 0,

    /// Unhandled
    PR_EVENT_UNHANDLED
};

/// Format of a task message handler function
typedef enum process_event_response(*process_event_func_t)(ke_msg_id_t const msgid,
                                                           void const *param,
                                                           ke_task_id_t const dest_id,
                                                           ke_task_id_t const src_id,
                                                           enum ke_msg_status_tag *msg_ret);

#if defined (__DA14531__) && !defined (__EXCLUDE_ROM_APP_TASK__)

#include "app_prf_perm_types.h"

#if (BLE_APP_SEC)
#include "app_security.h"
#endif

/// APP Task configuration
typedef struct
{
    /// app_easy_gap_dev_configure() function
    void (*app_easy_gap_dev_configure)(void);

    /// app_db_init_next() function
    bool (*app_db_init_next)(void);

    /// User app callbacks
    const struct app_callbacks *user_app_callbacks;

    /// catch_rest_func() function
    const catch_rest_event_func_t catch_rest_func;

    /// APP process handlers
    const process_event_func_t *app_process_handlers;

    /// APP env
    struct app_env_tag *app_env;

    /// APP security env
    struct app_sec_bond_data_env_tag *app_sec_env;

    /// Profile access rights
    app_prf_srv_sec_t *app_prf_srv_perm;
} rom_app_task_cfg_t;

#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Application entry point handler.
 * @param[in] msgid       Message Id
 * @param[in] param       Pointer to message
 * @param[in] dest_id     Destination task Id
 * @param[in] src_id      Source task Id
 * @return Message status
 ****************************************************************************************
 */
int app_entry_point_handler(ke_msg_id_t const msgid,
                            void const *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id);
                                       
/**
 ****************************************************************************************
 * @brief Application standard process event handler.
 * @param[in] msgid       Message Id
 * @param[in] param       Pointer to message
 * @param[in] src_id      Source task Id
 * @param[in] dest_id     Destination task Id
 * @param[in] msg_ret     Message status returned
 * @param[in] handlers    Pointer to message handlers
   @param[in] handler_num Handler number
 * @return process_event_response PR_EVENT_HANDLED or PR_EVENT_UNHANDLED
 ****************************************************************************************
 */
enum process_event_response app_std_process_event(ke_msg_id_t const msgid,
                                                  void const *param,
                                                  ke_task_id_t const src_id,
                                                  ke_task_id_t const dest_id,
                                                  enum ke_msg_status_tag *msg_ret,
                                                  const struct ke_msg_handler *handlers,
                                                  const int handler_num);

#endif // _APP_ENTRY_POINT_H_

///@}
///@}
