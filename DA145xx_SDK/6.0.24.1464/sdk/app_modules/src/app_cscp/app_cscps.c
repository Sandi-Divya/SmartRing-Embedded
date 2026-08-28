/**
 ****************************************************************************************
 *
 * @file app_cscps.c
 *
 * @brief Cycling Speed and Cadence Profile Server application.
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

#if (BLE_CSC_SENSOR)
#include "app.h"
#include "app_task.h"
#include "app_cscps.h"
#include "app_prf_perm_types.h"
#include "prf_utils.h"
#include "user_callback_config.h"

#include "cscps.h"
#include "cscps_task.h"

// Define some default features when flag is not defined
#ifndef APP_CSCP_FEATURES
#define APP_CSCP_FEATURES           CSCP_FEAT_ALL_SUPP
#endif

#ifndef APP_SENSOR_LOC
#define APP_SENSOR_LOC              CSCP_LOC_FRONT_WHEEL
#endif

#ifndef APP_SENSOR_LOC_SUP
#define APP_SENSOR_LOC_SUP          CSCP_LOC_FRONT_WHEEL
#endif

#ifndef APP_WHEEL_REV
#define APP_WHEEL_REV               0
#endif

#ifndef APP_CSCP_CSCS_CHAR
#define APP_CSCP_CSCS_CHAR          CSCP_CSCS_CSC_MEAS_CHAR
#endif

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_cscps_init(void)
{
    return;
}

void app_cscps_create_db(void)
{
    struct cscps_db_cfg* db_cfg;
    const struct att_incl_desc *incl = NULL;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct cscps_db_cfg));

    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_CSCPS);
    req->prf_task_id = TASK_ID_CSCPS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    db_cfg = (struct cscps_db_cfg* ) req->param;

    db_cfg->csc_feature = APP_CSCP_FEATURES;
    db_cfg->sensor_loc = APP_SENSOR_LOC;
    db_cfg->sensor_loc_supp = APP_SENSOR_LOC_SUP;
    db_cfg->wheel_rev = APP_WHEEL_REV;
 
    ke_msg_send(req);
}

void app_cscps_enable(uint8_t conidx)
{
    struct cscps_env_tag *cscp_env = PRF_ENV_GET(CSCPS, cscps);

    struct cscps_enable_req * req = KE_MSG_ALLOC(CSCPS_ENABLE_REQ,
                                                 prf_src_task_get(&cscp_env->prf_env, conidx),
                                                 TASK_APP,
                                                 cscps_enable_req);

    req->sc_ctnl_pt_ntf_cfg = CSCP_PRF_CFG_FLAG_SC_CTNL_PT_IND;
    req->csc_meas_ntf_cfg = CSCP_PRF_CFG_FLAG_CSC_MEAS_NTF;
    req->conidx = conidx;

    ke_msg_send(req);
}

void app_cscps_ntf_csc_meas_req(uint8_t conidx, const struct cscp_csc_meas *meas)
{
    struct cscps_env_tag *cscp_env = PRF_ENV_GET(CSCPS, cscps);

    struct cscps_ntf_csc_meas_req *req = KE_MSG_ALLOC(CSCPS_NTF_CSC_MEAS_REQ,
                                                  prf_src_task_get(&cscp_env->prf_env, conidx),
                                                  TASK_APP,
                                                  cscps_ntf_csc_meas_req);
    
    req->cumul_crank_rev = meas->cumul_crank_rev;
    req->flags = meas->flags;
    req->last_crank_evt_time = meas->last_crank_evt_time;
    req->last_wheel_evt_time = meas->last_wheel_evt_time;
    req->wheel_rev = meas->cumul_wheel_rev;

    ke_msg_send(req);
}

void app_cscps_sc_ctnl_pt_req_ind(uint8_t conidx, const struct cscps_sc_ctnl_pt_req_ind *param)
{
    struct cscps_env_tag *cscp_env = PRF_ENV_GET(CSCPS, cscps);

    struct cscps_sc_ctnl_pt_req_ind *req = KE_MSG_ALLOC(CSCPS_SC_CTNL_PT_REQ_IND,
                                                  prf_src_task_get(&cscp_env->prf_env, conidx),
                                                  TASK_APP,
                                                  cscps_sc_ctnl_pt_req_ind);

    req->conidx = conidx;
    req->op_code = param->op_code;
    req->value.cumul_value = param->value.cumul_value;

    ke_msg_send(req);
}

void app_cscps_sc_ctnl_pt_cfm(uint8_t conidx, const struct cscps_sc_ctnl_pt_cfm *param)
{
    struct cscps_env_tag *cscp_env = PRF_ENV_GET(CSCPS, cscps);

    struct cscps_sc_ctnl_pt_cfm *req = KE_MSG_ALLOC(CSCPS_SC_CTNL_PT_CFM,
                                                  prf_src_task_get(&cscp_env->prf_env, conidx),
                                                  TASK_APP,
                                                  cscps_sc_ctnl_pt_cfm);

    req->conidx = conidx;
    req->op_code = param->op_code;
    req->status = param->status;
    req->value.cumul_wheel_rev = param->value.cumul_wheel_rev;

    ke_msg_send(req);
}

void app_cscps_cfg_ntfind_ind(uint8_t conidx, const struct cscps_cfg_ntfind_ind *param)
{
    struct cscps_env_tag *cscp_env = PRF_ENV_GET(CSCPS, cscps);

    struct cscps_cfg_ntfind_ind *req = KE_MSG_ALLOC(CSCPS_CFG_NTFIND_IND,
                                                    prf_src_task_get(&cscp_env->prf_env, conidx),
                                                    TASK_APP,
                                                    cscps_cfg_ntfind_ind);

    req->conidx = param->conidx;
    req->char_code = param->char_code;
    req->ntf_cfg = param->ntf_cfg;

    ke_msg_send(req);
}

#endif // (BLE_CSC_SENSOR)

#endif // BLE_APP_PRESENT

/// @} APP
