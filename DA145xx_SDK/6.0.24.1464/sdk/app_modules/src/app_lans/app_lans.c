/**
 ****************************************************************************************
 *
 * @file app_lans.c
 *
 * @brief Location and Navigation Profile Server application.
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

#if (BLE_LN_SENSOR)
#include "app.h"
#include "app_task.h"
#include "app_lans.h"
#include "app_prf_perm_types.h"
#include "prf_utils.h"
#include "user_callback_config.h"

#include "lans.h"
#include "lans_task.h"

// Define some default features when flag is not defined
#ifndef APP_LANS_FEATURES
#define APP_LANS_FEATURES           LANP_FEAT_ALL_SUPP
#endif

#ifndef APP_LANS_PROFILE_CONFIG
#define APP_LANS_PROFILE_CONFIG     LANS_CTNL_PT_CHAR_SUPP_FLAG | \
                                    LANS_NAVIGATION_SUPP_FLAG
#endif

#ifndef APP_LANS_NTF_CONFIG
#define APP_LANS_NTF_CONFIG         LANS_PRF_CFG_FLAG_LOC_SPEED_NTF | \
                                    LANS_PRF_CFG_FLAG_LN_CTNL_PT_IND | \
                                    LANS_PRF_CFG_FLAG_NAVIGATION_NTF
#endif

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */


/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_lans_init(void)
{
    return;
}

void app_lans_create_db(void)
{
    struct lans_db_cfg* db_cfg;
    const struct att_incl_desc *incl = NULL;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct lans_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_LANS);
    req->prf_task_id = TASK_ID_LANS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;
    db_cfg = (struct lans_db_cfg* ) req->param;
    db_cfg->ln_feature = APP_LANS_FEATURES;
    db_cfg->prfl_config = APP_LANS_PROFILE_CONFIG;

    // Send the message
    KE_MSG_SEND(req);
}

void app_lans_enable(uint8_t conidx)
{
    struct lans_env_tag *lans_env = PRF_ENV_GET(LANS, lans);
    // Allocate the message
    struct lans_enable_req * req = KE_MSG_ALLOC(LANS_ENABLE_REQ,
                                                prf_src_task_get(&lans_env->prf_env, conidx),
                                                TASK_APP,
                                                lans_enable_req);

    req->prfl_ntf_ind_cfg = APP_LANS_NTF_CONFIG;
    req->conidx = conidx;

    KE_MSG_SEND(req);
}

void app_lans_send_loc_speed_data(uint8_t conidx, const struct lanp_loc_speed *data)
{
    struct lans_env_tag *lans_env = PRF_ENV_GET(LANS, lans);
    // Indicate the location and speed data
    struct lans_ntf_loc_speed_req *req = KE_MSG_ALLOC(LANS_NTF_LOC_SPEED_REQ,
                                                  prf_src_task_get(&lans_env->prf_env, conidx),
                                                  TASK_APP,
                                                  lans_ntf_loc_speed_req);

    memcpy(&req->parameters, data, sizeof(struct lanp_loc_speed));

    KE_MSG_SEND(req);
}

void app_lans_send_pos_q_data(uint8_t conidx, const struct lanp_posq *data)
{
    struct lans_env_tag *lans_env = PRF_ENV_GET(LANS, lans);
    // Indicate the position quality data
    struct lans_upd_pos_q_req *req = KE_MSG_ALLOC(LANS_UPD_POS_Q_REQ,
                                                  prf_src_task_get(&lans_env->prf_env, conidx),
                                                  TASK_APP,
                                                  lans_upd_pos_q_req);

    memcpy(&req->parameters, data, sizeof(struct lanp_posq));

    KE_MSG_SEND(req);
}

void app_lans_send_nav_data(uint8_t conidx, const struct lanp_navigation *data)
{
    struct lans_env_tag *lans_env = PRF_ENV_GET(LANS, lans);
    // Indicate the navigation data
    struct lans_ntf_navigation_req *req = KE_MSG_ALLOC(LANS_NTF_NAVIGATION_REQ,
                                                  prf_src_task_get(&lans_env->prf_env, conidx),
                                                  TASK_APP,
                                                  lans_ntf_navigation_req);

    memcpy(&req->parameters, data, sizeof(struct lanp_navigation));

    KE_MSG_SEND(req);
}

void app_lans_ln_ctnl_pt_ind(uint8_t conidx, const struct lan_ln_ctnl_pt_req *data)
{
    struct lans_env_tag *lans_env = PRF_ENV_GET(LANS, lans);
    // Indicate the control point data
    struct lans_ln_ctnl_pt_req_ind *req = KE_MSG_ALLOC(LANS_LN_CTNL_PT_REQ_IND,
                                                  prf_src_task_get(&lans_env->prf_env, conidx),
                                                  TASK_APP,
                                                  lans_ln_ctnl_pt_req_ind);

    memcpy(req, data, sizeof(struct lan_ln_ctnl_pt_req));

    KE_MSG_SEND(req);
}

void app_lans_cntl_point_operation_cfm(uint8_t conidx, const struct lanp_ln_ctnl_pt_rsp *rsp)
{
    struct lans_env_tag *lans_env = PRF_ENV_GET(LANS, lans);
    // Confirmation the control point data
    struct lans_ln_ctnl_pt_cfm* cfm = KE_MSG_ALLOC(LANS_LN_CTNL_PT_CFM,
                                                 prf_src_task_get(&lans_env->prf_env, conidx),
                                                 TASK_APP, lans_ln_ctnl_pt_cfm);

    cfm->conidx = conidx;
    cfm->op_code = rsp->req_op_code;
    cfm->status = rsp->resp_value;

    memcpy(&cfm->value, &rsp->value, sizeof(union lanp_ctnl_pt_rsp_val));

    KE_MSG_SEND(cfm);
}

#endif // (BLE_LN_SENSOR)

#endif // BLE_APP_PRESENT

/// @} APP
