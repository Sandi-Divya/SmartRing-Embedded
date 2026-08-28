/**
 ****************************************************************************************
 *
 * @file app_tips.c
 *
 * @brief Time Profile Server application.
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

#if (BLE_TIP_SERVER)
#include "app.h"
#include "app_task.h"
#include "app_tips.h"
#include "app_prf_perm_types.h"
#include "prf_utils.h"
#include "user_callback_config.h"

#include "tips.h"
#include "tips_task.h"

// Define some default features when flag is not defined

#ifndef APP_TIPS_FEATURES
#define APP_TIPS_FEATURES               TIPS_CTS_CURR_TIME_SUP | \
                                        TIPS_CTS_LOC_TIME_INFO_SUP | \
                                        TIPS_CTS_REF_TIME_INFO_SUP | \
                                        TIPS_NDCS_SUP | \
                                        TIPS_RTUS_SUP | \
                                        TIPS_CTS_CURRENT_TIME_CFG
#endif

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */


/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_tips_init(void)
{
    return;
}

void app_tips_create_db(void)
{
    struct tips_db_cfg* db_cfg;
    const struct att_incl_desc *incl = NULL;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct tips_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_TIPS);
    req->prf_task_id = TASK_ID_TIPS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;
    db_cfg = (struct tips_db_cfg* ) req->param;
    db_cfg->features = APP_TIPS_FEATURES;

    // Send the message
    KE_MSG_SEND(req);
}

void app_tips_enable(uint8_t conidx)
{
    struct tips_env_tag *tips_env = PRF_ENV_GET(TIPS, tips);
    // Allocate the message
    struct tips_enable_req * req = KE_MSG_ALLOC(TIPS_ENABLE_REQ,
                                                prf_src_task_get(&tips_env->prf_env, conidx),
                                                TASK_APP,
                                                tips_enable_req);

    req->current_time_ntf_en = PRF_CLI_START_NTF;

    KE_MSG_SEND(req);
}

void app_tips_curr_time(uint8_t conidx, const struct tip_curr_time *time)
{
    struct tips_env_tag *tips_env = PRF_ENV_GET(TIPS, tips);
    // Indicate the current time
    struct tips_upd_curr_time_req *req = KE_MSG_ALLOC(TIPS_UPD_CURR_TIME_REQ,
                                                  prf_src_task_get(&tips_env->prf_env, conidx),
                                                  TASK_APP,
                                                  tips_upd_curr_time_req);

    memcpy(&req->current_time, time, sizeof(struct tip_curr_time));

    KE_MSG_SEND(req);
}

void app_tips_rd_cfm(uint8_t conidx, const struct tips_rd_cfm *data)
{
    struct tips_env_tag *tips_env = PRF_ENV_GET(TIPS, tips);
    // Indicate the read configuration
    struct tips_rd_cfm *cfm = KE_MSG_ALLOC(TIPS_RD_CFM,
                                                  prf_src_task_get(&tips_env->prf_env, conidx),
                                                  TASK_APP,
                                                  tips_rd_cfm);
    
    memcpy(cfm, data, sizeof(struct tips_rd_cfm));

    KE_MSG_SEND(cfm);
}

#endif // (BLE_TIP_SERVER)

#endif // BLE_APP_PRESENT

/// @} APP
