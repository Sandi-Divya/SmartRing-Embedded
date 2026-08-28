/**
 ****************************************************************************************
 *
 * @file app_ctss.c
 *
 * @brief Current Time Service Application entry point
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
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_PRESENT)

#if (BLE_CTS_SERVER)
#include "app_ctss.h"                // Current Time Service Application Definitions
#include "app.h"                     // Application Definitions
#include "app_task.h"                // Application Task Definitions
#include "ctss_task.h"               // Health Thermometer Functions
#include "app_prf_perm_types.h"
#include "user_profiles_config.h"

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_ctss_init(void)
{
    // Nothing to do
}

void app_ctss_create_db(void)
{
    struct ctss_db_cfg *db_cfg;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct ctss_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_CTSS);
    req->prf_task_id = TASK_ID_CTSS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct ctss_db_cfg *) req->param;
    db_cfg->features = APP_CTS_FEATURES;

    // Send the message
    KE_MSG_SEND(req);
}

void app_ctss_notify_current_time(const struct cts_curr_time *curr_time)
{
    struct ctss_ntf_curr_time_req *req = KE_MSG_ALLOC(CTSS_NTF_CURR_TIME_REQ,
                                                      prf_get_task_from_id(TASK_ID_CTSS),
                                                      TASK_APP,
                                                      ctss_ntf_curr_time_req);

    // Fill message
    req->current_time = *curr_time;

    // Send the message
    KE_MSG_SEND(req);
}

void app_ctss_set_local_time_info(const struct cts_loc_time_info *local_time_info)
{
    struct ctss_upd_local_time_info_req *req = KE_MSG_ALLOC(CTSS_UPD_LOCAL_TIME_INFO_REQ,
                                                            prf_get_task_from_id(TASK_ID_CTSS),
                                                            TASK_APP,
                                                            ctss_upd_local_time_info_req);

    // Fill message
    req->loc_time_info = *local_time_info;

    // Send the message
    KE_MSG_SEND(req);
}

#endif // (BLE_CTS_SERVER)

#endif // BLE_APP_PRESENT

/// @} APP
