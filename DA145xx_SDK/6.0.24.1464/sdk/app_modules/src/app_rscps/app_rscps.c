/**
 ****************************************************************************************
 *
 * @file app_rscps.c
 *
 * @brief Running Speed and Cadence Profile Sensor application.
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

#if (BLE_RSC_SENSOR)
#include "app.h"
#include "app_task.h"
#include "app_rscps.h"
#include "app_prf_perm_types.h"
#include "prf_utils.h"
#include "user_callback_config.h"

#include "rscps.h"
#include "rscps_task.h"

// Define some default features when flag is not defined
#ifndef APP_RSCPS_FEATURES
#define APP_RSCPS_FEATURES      RSCP_FEAT_ALL_SUPP
#endif

#ifndef APP_RSCPS_SENSOR_LOC
#define APP_RSCPS_SENSOR_LOC    RSCP_LOC_CHEST
#endif

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */


/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_rscps_init(void)
{
    return;
}

void app_rscps_create_db(void)
{
    struct rscps_db_cfg* db_cfg;
    const struct att_incl_desc *incl = NULL;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct rscps_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_RSCPS);
    req->prf_task_id = TASK_ID_RSCPS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;
    db_cfg = (struct rscps_db_cfg* ) req->param;
    db_cfg->rsc_feature = APP_RSCPS_FEATURES;
    db_cfg->sensor_loc = APP_RSCPS_SENSOR_LOC;

    // Send the message
    KE_MSG_SEND(req);
}

void app_rscps_enable(uint8_t conidx)
{
    struct rscps_env_tag *rscps_env = PRF_ENV_GET(RSCPS, rscps);
    // Allocate the message
    struct rscps_enable_req * req = KE_MSG_ALLOC(RSCPS_ENABLE_REQ,
                                                prf_src_task_get(&rscps_env->prf_env, conidx),
                                                TASK_APP,
                                                rscps_enable_req);

    req->rsc_meas_ntf_cfg = RSCP_PRF_CFG_FLAG_RSC_MEAS_NTF;
    req->sc_ctnl_pt_ntf_cfg = RSCP_PRF_CFG_FLAG_SC_CTNL_PT_IND;
    req->conidx = conidx;

    KE_MSG_SEND(req);
}

void app_rscps_send_meas(uint8_t conidx, const struct rscp_rsc_meas *meas)
{
    struct rscps_env_tag *rscps_env = PRF_ENV_GET(RSCPS, rscps);
    // Indicate the measurement
    struct rscps_ntf_rsc_meas_req *req = KE_MSG_ALLOC(RSCPS_NTF_RSC_MEAS_REQ,
                                                  prf_src_task_get(&rscps_env->prf_env, conidx),
                                                  TASK_APP,
                                                  rscps_ntf_rsc_meas_req);

    memcpy(req, meas, sizeof(struct rscp_rsc_meas));

    KE_MSG_SEND(req); 
}

void app_rscps_cntl_point_operation_cfm(uint8_t conidx, const struct rscp_sc_ctnl_pt_rsp *rsp)
{
    struct rscps_env_tag *rscps_env = PRF_ENV_GET(RSCPS, rscps);
    // Confirmation the control point data
    struct rscps_sc_ctnl_pt_cfm* cfm = KE_MSG_ALLOC(RSCPS_SC_CTNL_PT_CFM,
                                                 prf_src_task_get(&rscps_env->prf_env, conidx),
                                                 TASK_APP, rscps_sc_ctnl_pt_cfm);

    cfm->conidx = conidx;
    cfm->op_code = rsp->req_op_code;
    cfm->status = rsp->resp_value;
    cfm->value.supp_sensor_loc = rsp->supp_loc;
    cfm->value.sensor_loc = rsp->sensor_loc;
    
    KE_MSG_SEND(cfm);
}

#endif // (BLE_RSC_SENSOR)

#endif // BLE_APP_PRESENT

/// @} APP
