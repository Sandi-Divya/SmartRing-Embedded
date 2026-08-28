/**
 ****************************************************************************************
 *
 * @file app_wsss.c
 *
 * @brief Weight Scale Profile Server application.
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

#if (BLE_WSS_SERVER)
#include "app.h"
#include "app_task.h"
#include "app_wsss.h"
#include "app_prf_perm_types.h"
#include "prf_utils.h"
#include "user_callback_config.h"

#include "wsss.h"
#include "wsss_task.h"

// Define some default features when flag is not defined
#ifndef APP_WSSS_FEATURES
#define APP_WSSS_FEATURES WSSS_FEAT_TIME_STAMP_SUPPORTED | \
                          WSSS_FEAT_BMI_SUPPORTED | \
                          WSSS_FEAT_HT_DISPLAY_1MM_ACC | \
                          WSSS_FEAT_WT_DISPLAY_500G_ACC
#endif

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */
static bool measurement_ind_en __SECTION_ZERO("retention_mem_area0");

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_wsss_init(void)
{
    return;
}

void app_wsss_create_db(void)
{
    struct wsss_db_cfg* db_cfg;
    const struct att_incl_desc *incl = NULL;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct wsss_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_WSSS);
    req->prf_task_id = TASK_ID_WSSS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;
    db_cfg = (struct wsss_db_cfg* ) req->param;
    db_cfg->ws_feature = APP_WSSS_FEATURES;

    if(user_app_wsss_cb.on_wsss_bcs_reference_req)
    {
        incl = user_app_wsss_cb.on_wsss_bcs_reference_req();
    }

    if(incl != NULL)
    {
        db_cfg->include_bcs_instance = true;
        memcpy(&db_cfg->bcs_ref, incl, sizeof(db_cfg->bcs_ref));
    }

    // Send the message
    KE_MSG_SEND(req);
}

void app_wsss_enable(uint8_t conidx)
{
    struct wsss_env_tag *wsss_env = PRF_ENV_GET(WSSS, wsss);
    // Allocate the message
    struct wsss_enable_req * req = KE_MSG_ALLOC(WSSS_ENABLE_REQ,
                                                prf_src_task_get(&wsss_env->prf_env, conidx),
                                                TASK_APP,
                                                wsss_enable_req);

    req->wt_meas_ind_en = measurement_ind_en ? GATT_CCC_START_IND : GATT_CCC_STOP_NTFIND;

    KE_MSG_SEND(req);
}

void app_wsss_send_measurement(uint8_t conidx, const struct wss_wt_meas *meas)
{
    struct wsss_env_tag *wsss_env = PRF_ENV_GET(WSSS, wsss);
    // Indicate the measurement
    struct wsss_meas_send_req *req = KE_MSG_ALLOC(WSSS_MEAS_SEND_REQ,
                                                  prf_src_task_get(&wsss_env->prf_env, conidx),
                                                  TASK_APP,
                                                  wsss_meas_send_req);

    memcpy(&req->meas_val, meas, sizeof(*meas));

    KE_MSG_SEND(req);

}

void app_wsss_set_initial_measurement_ind_cfg(bool enabled)
{
    measurement_ind_en = enabled;
}

#endif // (BLE_WSS_SERVER)

#endif // BLE_APP_PRESENT

/// @} APP
