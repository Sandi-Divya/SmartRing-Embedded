/**
 ****************************************************************************************
 *
 * @file app_htpt.c
 *
 * @brief Health Thermometer Profile Server application.
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

#if (BLE_HT_THERMOM)
#include "app.h"
#include "app_task.h"
#include "app_htpt.h"
#include "app_prf_perm_types.h"
#include "prf_utils.h"
#include "user_callback_config.h"

#include "htpt.h"
#include "htpt_task.h"

// Define some default features when flag is not defined
#ifndef APP_HTPT_FEATURES
#define APP_HTPT_FEATURES            HTPT_ALL_FEAT_SUP
#endif

#ifndef APP_HTPT_MEAS_INTERVAL
#define APP_HTPT_MEAS_INTERVAL       5
#endif

#ifndef APP_HTPT_VALID_RANGE_MIN
#define APP_HTPT_VALID_RANGE_MIN     1
#endif

#ifndef APP_HTPT_VALID_RANGE_MAX
#define APP_HTPT_VALID_RANGE_MAX     10
#endif

#ifndef APP_HTPT_TEMP_TYPE
#define APP_HTPT_TEMP_TYPE           HTP_TYPE_ARMPIT
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

void app_htpt_init(void)
{
    return;
}

void app_htpt_create_db(void)
{
    struct htpt_db_cfg* db_cfg;
    const struct att_incl_desc *incl = NULL;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct htpt_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_HTPT);
    req->prf_task_id = TASK_ID_HTPT;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    db_cfg = (struct htpt_db_cfg* ) req->param;

    db_cfg->features = APP_HTPT_FEATURES;
    db_cfg->meas_intv = APP_HTPT_MEAS_INTERVAL;
    db_cfg->valid_range_min = APP_HTPT_VALID_RANGE_MIN;
    db_cfg->valid_range_max = APP_HTPT_VALID_RANGE_MAX;
    db_cfg->temp_type = APP_HTPT_TEMP_TYPE;
 
    ke_msg_send(req);
}

void app_htpt_enable(uint8_t conidx)
{
    struct htpt_env_tag *htpt_env = PRF_ENV_GET(HTPT, htpt);
    // Allocate the message
    struct htpt_enable_req * req = KE_MSG_ALLOC(HTPT_ENABLE_REQ,
                                                prf_src_task_get(&htpt_env->prf_env, conidx),
                                                TASK_APP,
                                                htpt_enable_req);

    req->ntf_ind_cfg = measurement_ind_en ? GATT_CCC_START_IND : GATT_CCC_STOP_NTFIND;
    req->conidx = conidx;

    ke_msg_send(req);
}

void app_htpt_send_measurement(uint8_t conidx, uint8_t stable_meas, const struct htp_temp_meas *meas)
{
    struct htpt_env_tag *htpt_env = PRF_ENV_GET(HTPT, htpt);
    // Indicate the measurement
    struct htpt_temp_send_req *req = KE_MSG_ALLOC(HTPT_TEMP_SEND_REQ,
                                                  prf_src_task_get(&htpt_env->prf_env, conidx),
                                                  TASK_APP,
                                                  htpt_temp_send_req);
    
    req->stable_meas = stable_meas ? true : false;

    memcpy(&req->temp_meas, meas, sizeof(*meas));

    ke_msg_send(req);
}

void app_htpt_send_measurement_interval(uint8_t conidx, uint16_t meas_intv)
{
    struct htpt_env_tag *htpt_env = PRF_ENV_GET(HTPT, htpt);
    // Indicate the measurement interval
    struct htpt_meas_intv_upd_req *req = KE_MSG_ALLOC(HTPT_MEAS_INTV_UPD_REQ,
                                                  prf_src_task_get(&htpt_env->prf_env, conidx),
                                                  TASK_APP,
                                                  htpt_meas_intv_upd_req);

    req->meas_intv = meas_intv;

    ke_msg_send(req);
}

void app_htpt_send_measurement_interval_chg_cfm(uint8_t conidx, uint8_t status)
{
    struct htpt_env_tag *htpt_env = PRF_ENV_GET(HTPT, htpt);
    // Confirm message of new measurement interval value requested by a peer device
    struct htpt_meas_intv_chg_cfm *req = KE_MSG_ALLOC(HTPT_MEAS_INTV_CHG_CFM,
                                                  prf_src_task_get(&htpt_env->prf_env, conidx),
                                                  TASK_APP,
                                                  htpt_meas_intv_chg_cfm);

    req->status = status;

    ke_msg_send(req);
}

void app_htpt_set_initial_measurement_ind_cfg(bool enabled)
{
    measurement_ind_en = enabled;
}

#endif // (BLE_HT_THERMOM)

#endif // BLE_APP_PRESENT

/// @} APP
