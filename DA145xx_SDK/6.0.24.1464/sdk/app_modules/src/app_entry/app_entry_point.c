/**
 ****************************************************************************************
 *
 * @file app_entry_point.c
 *
 * @brief Application entry point source code.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "app_api.h"
#include "user_callback_config.h"
#include "user_modules_config.h"

#if (!EXCLUDE_DLG_GAP)
#include "app_task.h"
#endif

#if (!EXCLUDE_DLG_TIMER)
#include "app_easy_timer.h"
#endif

#if (!EXCLUDE_DLG_MSG)
#include "app_easy_msg_utils.h"
#endif

#if ((BLE_APP_SEC) && (!EXCLUDE_DLG_SEC))
#include "app_security_task.h"
#endif

#if ((BLE_DIS_SERVER) && (!EXCLUDE_DLG_DISS))
#include "app_diss_task.h"
#endif

#if ((BLE_PROX_REPORTER) && (!EXCLUDE_DLG_PROXR))
#include "app_proxr_task.h"
#endif

#if ((BLE_BATT_SERVER) && (!EXCLUDE_DLG_BASS))
#include "app_bass_task.h"
#endif

#if ((BLE_CTS_SERVER) && (!EXCLUDE_DLG_CTSS))
#include "app_ctss_task.h"
#endif

#if ((BLE_CTS_CLIENT) && (!EXCLUDE_DLG_CTSC))
#include "app_ctsc_task.h"
#endif

#if (((BLE_FINDME_TARGET) && (!EXCLUDE_DLG_FINDT)) || ((BLE_FINDME_LOCATOR) && (!EXCLUDE_DLG_FINDL)))
#include "app_findme_task.h"
#endif

#if ((BLE_BMS_SERVER) && (!EXCLUDE_DLG_BMSS))
#include "app_bmss_task.h"
#endif

#if ((BLE_BCS_SERVER) && (!EXCLUDE_DLG_BCSS))
#include "app_bcss_task.h"
#endif

#if ((BLE_UDS_SERVER) && (!EXCLUDE_DLG_UDSS))
#include "app_udss_task.h"
#endif

#if ((BLE_GATT_CLIENT) && (!EXCLUDE_DLG_GATTC))
#include "app_gattc_task.h"
#endif

#if ((BLE_SUOTA_RECEIVER) && (!EXCLUDE_DLG_SUOTAR))
#include "app_suotar_task.h"
#endif

#if (BLE_CUSTOM_SERVER)
#include "app_customs_task.h"
#endif

#if ((BLE_ANC_CLIENT) && (!EXCLUDE_DLG_ANCC))
#include "app_ancc_task.h"
#endif

#if ((BLE_WSS_SERVER) && (!EXCLUDE_DLG_WSSS))
#include "app_wsss_task.h"
#endif

#if ((BLE_GL_SENSOR) && (!EXCLUDE_DLG_GLPS))
#include "app_glps_task.h"
#endif

#if ((BLE_LN_SENSOR) && (!EXCLUDE_DLG_LANS))
#include "app_lans_task.h"
#endif

#if ((BLE_PAS_SERVER) && (!EXCLUDE_DLG_PASPS))
#include "app_pasps_task.h"
#endif

#if ((BLE_HID_DEVICE) && (!EXCLUDE_DLG_HOGPD))
#include "app_hogpd_task.h"
#endif

#if ((BLE_RSC_SENSOR) && (!EXCLUDE_DLG_RSCPS))
#include "app_rscps_task.h"
#endif

#if ((BLE_HR_SENSOR) && (!EXCLUDE_DLG_HRPS))
#include "app_hrps_task.h"
#endif

#if ((BLE_TIP_SERVER) && (!EXCLUDE_DLG_TIPS))
#include "app_tips_task.h"
#endif

#if ((BLE_BP_SENSOR) && (!EXCLUDE_DLG_BLPS))
#include "app_blps_task.h"
#endif

#if ((BLE_HT_THERMOM) && (!EXCLUDE_DLG_HTPT))
#include "app_htpt_task.h"
#endif

#if ((BLE_CGM_SERVER) && (!EXCLUDE_CGM_SERVER))
#include "app_cgms_task.h"
#endif

#if ((BLE_CSC_SENSOR) && (!EXCLUDE_DLG_CSCPS))
#include "app_cscps_task.h"
#endif

#if ((BLE_SP_SERVER) && (!EXCLUDE_DLG_SCPPS))
#include "app_scpps_task.h"
#endif

#if ((BLE_AN_SERVER) && (!EXCLUDE_DLG_ANPS))
#include "app_anps_task.h"
#endif

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

const process_event_func_t app_process_handlers[] = {

#if (!EXCLUDE_DLG_GAP)
    (process_event_func_t) APP_GAP_PROCESS_HANDLER_FUNC,
#endif

#if (!EXCLUDE_DLG_TIMER)
    (process_event_func_t) app_timer_api_process_handler,
#endif

#if (!EXCLUDE_DLG_MSG)
    (process_event_func_t) app_msg_utils_api_process_handler,
#endif

#if ((BLE_APP_SEC) && (!EXCLUDE_DLG_SEC))
    (process_event_func_t) app_sec_process_handler,
#endif

#if ((BLE_DIS_SERVER) && (!EXCLUDE_DLG_DISS))
    (process_event_func_t) app_diss_process_handler,
#endif

#if ((BLE_PROX_REPORTER) && (!EXCLUDE_DLG_PROXR))
    (process_event_func_t) app_proxr_process_handler,
#endif

#if ((BLE_BATT_SERVER) && (!EXCLUDE_DLG_BASS))
    (process_event_func_t) app_bass_process_handler,
#endif

#if ((BLE_CTS_SERVER) && (!EXCLUDE_DLG_CTSS))
    (process_event_func_t) app_ctss_process_handler,
#endif

#if ((BLE_CTS_CLIENT) && (!EXCLUDE_DLG_CTSC))
    (process_event_func_t) app_ctsc_process_handler,
#endif

#if (((BLE_FINDME_TARGET) && (!EXCLUDE_DLG_FINDT)) || ((BLE_FINDME_LOCATOR) && (!EXCLUDE_DLG_FINDL)))
    (process_event_func_t) app_findme_process_handler,
#endif

#if ((BLE_BMS_SERVER) && (!EXCLUDE_DLG_BMSS))
    (process_event_func_t) app_bmss_process_handler,
#endif

#if ((BLE_BCS_SERVER) && (!EXCLUDE_DLG_BCSS))
    (process_event_func_t) app_bcss_process_handler,
#endif

#if ((BLE_ANC_CLIENT) && (!EXCLUDE_DLG_ANCC))
    (process_event_func_t) app_ancc_process_handler,
#endif

#if ((BLE_UDS_SERVER) && (!EXCLUDE_DLG_UDSS))
    (process_event_func_t) app_udss_process_handler,
#endif

#if ((BLE_PLX_SERVER) && (!EXCLUDE_DLG_PLXS))
    (process_event_func_t) app_plxs_process_handler,
#endif

#if ((BLE_CGM_SERVER))
    (process_event_func_t) app_cgms_process_handler,
#endif

#if ((BLE_SUOTA_RECEIVER) && (!EXCLUDE_DLG_SUOTAR))
    (process_event_func_t) app_suotar_process_handler,
#endif

#if ((BLE_CUSTOM1_SERVER) && (!EXCLUDE_DLG_CUSTS1))
    (process_event_func_t) app_custs1_process_handler,
#endif

#if ((BLE_CUSTOM2_SERVER) && (!EXCLUDE_DLG_CUSTS2))
    (process_event_func_t) app_custs2_process_handler,
#endif

#if ((BLE_WSS_SERVER) && (!EXCLUDE_DLG_WSSS))
    (process_event_func_t) app_wsss_process_handler,
#endif

#if ((BLE_GL_SENSOR) && (!EXCLUDE_DLG_GLPS))
    (process_event_func_t) app_glps_process_handler,
#endif

#if ((BLE_LN_SENSOR) && (!EXCLUDE_DLG_LANS))
    (process_event_func_t) app_lans_process_handler,
#endif

#if ((BLE_PAS_SERVER) && (!EXCLUDE_DLG_PASPS))
    (process_event_func_t) app_pasps_process_handler,
#endif

#if ((BLE_HID_DEVICE) && (!EXCLUDE_DLG_HOGPD))
    (process_event_func_t) app_hogpd_process_handler,
#endif

#if ((BLE_RSC_SENSOR) && (!EXCLUDE_DLG_RSCPS))
    (process_event_func_t) app_rscps_process_handler,
#endif

#if ((BLE_HR_SENSOR) && (!EXCLUDE_DLG_HRPS))
    (process_event_func_t) app_hrps_process_handler,
#endif

#if ((BLE_TIP_SERVER) && (!EXCLUDE_DLG_TIPS))
    (process_event_func_t) app_tips_process_handler,
#endif

#if ((BLE_BP_SENSOR) && (!EXCLUDE_DLG_BLPS))
    (process_event_func_t) app_blps_process_handler,
#endif

#if ((BLE_HT_THERMOM) && (!EXCLUDE_DLG_HTPT))
    (process_event_func_t) app_htpt_process_handler,
#endif

#if ((BLE_GATT_CLIENT) && (!EXCLUDE_DLG_GATTC))
    (process_event_func_t) app_gattc_process_handler,
#endif

#if ((BLE_CSC_SENSOR) && (!EXCLUDE_DLG_CSCPS))
    (process_event_func_t) app_cscps_process_handler,
#endif

#if ((BLE_SP_SERVER) && (!EXCLUDE_DLG_SCPPS))
    (process_event_func_t) app_scpps_process_handler,
#endif

#if ((BLE_AN_SERVER) && (!EXCLUDE_DLG_ANPS))
    (process_event_func_t) app_anps_process_handler,
#endif

#if defined (__DA14531__) && !defined (__EXCLUDE_ROM_APP_TASK__)
    #define END_PROCESS_HANDLER_TABLE       (0xF8)
    (process_event_func_t) END_PROCESS_HANDLER_TABLE,
#endif
};

const struct app_entry_catch_rest catch_rest = {
    .cb = app_process_catch_rest_cb,
};

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_APP_TASK__)

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

int app_entry_point_handler(ke_msg_id_t const msgid,
                            void const *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id)
{
    int i = 0;
    enum ke_msg_status_tag process_msg_handling_result;

    while (i < (sizeof(app_process_handlers) / sizeof(process_event_func_t)))
    {
        ASSERT_ERROR(app_process_handlers[i]);
         if (app_process_handlers[i](msgid, param, dest_id, src_id, &process_msg_handling_result) == PR_EVENT_HANDLED)
         {
             return (process_msg_handling_result);
         }
         i++;
    }

    //user cannot do anything else than consume the message
    CALLBACK_ARGS_4(catch_rest.cb, msgid, param, dest_id, src_id);

    return (KE_MSG_CONSUMED);
};

/**
 ****************************************************************************************
 * @brief Search for a message handler function.
 * @param[in] msg_id        Id of the message whose handler is under request
 * @param[in] handlers      Pointer to the message handler table
 * @param[in] handler_num   Range of the message handler table
 * @return The message handler function or null
 ****************************************************************************************
 */
static ke_msg_func_t handler_search(ke_msg_id_t const msg_id,
                                    const struct ke_msg_handler *handlers,
                                    const int handler_num)
{
    //table is empty
    if (handler_num == 0)
    {
        return NULL;
    }

    // Get the message handler function by parsing the message table
    for (int i = (handler_num-1); 0 <= i; i--)
    {
        if ((handlers[i].id == msg_id)
                || (handlers[i].id == KE_MSG_DEFAULT_HANDLER))
        {
            // If handler is NULL, message should not have been received in this state
            ASSERT_ERROR(handlers[i].func);

            return handlers[i].func;
        }
    }

    // If we execute this line of code, it means that we did not find the handler
    return NULL;
}

enum process_event_response app_std_process_event(ke_msg_id_t const msgid,
                                                  void const *param,
                                                  ke_task_id_t const src_id,
                                                  ke_task_id_t const dest_id,
                                                  enum ke_msg_status_tag *msg_ret,
                                                  const struct ke_msg_handler *handlers,
                                                  const int handler_num)
{
    ke_msg_func_t func = NULL;
    func = handler_search(msgid, handlers, handler_num);

    if (func != NULL)
    {
        *msg_ret = (enum ke_msg_status_tag) func(msgid, param, dest_id, src_id);
        return PR_EVENT_HANDLED;
    }
    else
    {
        return PR_EVENT_UNHANDLED;
    }
}

#else

extern bool app_db_init_next(void);

// Initialization of external variables used by ROM built-in SDK functions
const rom_app_task_cfg_t rom_app_task_cfg =
{
    .app_easy_gap_dev_configure = app_easy_gap_dev_configure,
    .app_db_init_next           = app_db_init_next,
    .user_app_callbacks         = &user_app_callbacks,
    .catch_rest_func            = app_process_catch_rest_cb,
    .app_process_handlers       = app_process_handlers,
    .app_env                    = app_env,
#if (BLE_APP_SEC)
    .app_sec_env                = app_sec_env,
#else
    .app_sec_env                = NULL,
#endif
    .app_prf_srv_perm           = app_prf_srv_perm,
};

#endif
