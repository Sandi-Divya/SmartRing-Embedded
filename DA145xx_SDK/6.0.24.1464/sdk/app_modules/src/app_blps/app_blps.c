/**
 ****************************************************************************************
 *
 * @file app_blps.c
 *
 * @brief Blood Pressure Profile Server application.
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

#if (BLE_BP_SENSOR)
#include "app.h"
#include "app_task.h"
#include "app_blps.h"
#include "app_prf_perm_types.h"
#include "prf_utils.h"
#include "user_callback_config.h"

#include "blps.h"
#include "blps_task.h"

// Define some default features when flag is not defined
#ifndef APP_BPS_FEATURES
#define APP_BPS_FEATURES            BPS_F_BODY_MVMT_DETECT_SUPPORTED | \
                                    BPS_F_CUFF_FIT_DETECT_SUPPORTED | \
                                    BPS_F_IRREGULAR_PULSE_DETECT_SUPPORTED | \
                                    BPS_F_PULSE_RATE_RANGE_DETECT_SUPPORTED | \
                                    BPS_F_MEAS_POS_DETECT_SUPPORTED | \
                                    BPS_F_MULTIPLE_BONDS_SUPPORTED
#endif

#ifndef APP_BPS_PROFILE_CONFIG
#define APP_BPS_PROFILE_CONFIG      BLPS_INTM_CUFF_PRESS_SUP
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

void app_blps_init(void)
{
    return;
}

void app_blps_create_db(void)
{
    struct blps_db_cfg *db_cfg;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct blps_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_BLPS);
    req->prf_task_id = TASK_ID_BLPS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct blps_db_cfg *) req->param;
    db_cfg->features = APP_BPS_FEATURES;
    db_cfg->prfl_cfg = APP_BPS_PROFILE_CONFIG;

    // Send the message
    ke_msg_send(req);
}

void app_blps_enable(uint8_t conidx)
{
    struct blps_env_tag *blps_env = PRF_ENV_GET(BLPS, blps);
    // Allocate the message
    struct blps_enable_req * req = KE_MSG_ALLOC(BLPS_ENABLE_REQ,
                                                prf_src_task_get(&blps_env->prf_env, conidx),
                                                TASK_APP,
                                                blps_enable_req);

    req->bp_meas_ind_en = measurement_ind_en ? GATT_CCC_START_IND : GATT_CCC_STOP_NTFIND;

    ke_msg_send(req);
}

void app_blps_send_measurement(uint8_t conidx, uint8_t flag_interm, const struct bps_bp_meas *meas)
{
    struct blps_env_tag *blps_env = PRF_ENV_GET(BLPS, blps);
    // Indicate the measurement
    struct blps_meas_send_req *req = KE_MSG_ALLOC(BLPS_MEAS_SEND_REQ,
                                                  prf_src_task_get(&blps_env->prf_env, conidx),
                                                  TASK_APP,
                                                  blps_meas_send_req);

    req->flag_interm_cp = flag_interm;

    memcpy(&req->meas_val, meas, sizeof(*meas));

    ke_msg_send(req);

}

void app_blps_set_initial_measurement_ind_cfg(bool enabled)
{
    measurement_ind_en = enabled;
}

#endif // (BLE_BP_SENSOR)

#endif // BLE_APP_PRESENT

/// @} APP
