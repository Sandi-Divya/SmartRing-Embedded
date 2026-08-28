/**
 ****************************************************************************************
 *
 * @file app_hrps.c
 *
 * @brief Heart Rate Profile Sensor application.
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

#if (BLE_HR_SENSOR)

#include "hrps.h"
#include "hrps_task.h"

#include "app.h"
#include "app_task.h"
#include "app_hrps.h"
#include "app_prf_perm_types.h"
#include "prf_utils.h"
#include "user_callback_config.h"


// Define some default features when flag is not defined
#ifndef APP_HRPS_FEATURES
#define APP_HRPS_FEATURES           HRS_F_BODY_SENSOR_LOCATION_SUPPORTED | \
                                    HRS_F_ENERGY_EXPENDED_SUPPORTED
#endif

#ifndef APP_HRPS_SENSOR_LOCATION
#define APP_HRPS_SENSOR_LOCATION    HRS_LOC_FINGER
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

void app_hrps_init(void)
{
    return;
}

void app_hrps_create_db(void)
{
    struct hrps_db_cfg* db_cfg;
    const struct att_incl_desc *incl = NULL;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct hrps_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_HRPS);
    req->prf_task_id = TASK_ID_HRPS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;
    db_cfg = (struct hrps_db_cfg* ) req->param;
    db_cfg->features = APP_HRPS_FEATURES;
    db_cfg->body_sensor_loc = APP_HRPS_SENSOR_LOCATION;

    // Send the message
    KE_MSG_SEND(req);
}

void app_hrps_enable(uint8_t conidx)
{
    struct hrps_env_tag *hrps_env = PRF_ENV_GET(HRPS, hrps);
    // Allocate the message
    struct hrps_enable_req * req = KE_MSG_ALLOC(HRPS_ENABLE_REQ,
                                                prf_src_task_get(&hrps_env->prf_env, conidx),
                                                TASK_APP,
                                                hrps_enable_req);

    req->hr_meas_ntf = measurement_ind_en ? GATT_CCC_START_IND : GATT_CCC_STOP_NTFIND;

    KE_MSG_SEND(req);
}

void app_hrps_send_measurement(uint8_t conidx, const struct hrs_hr_meas *meas)
{
    struct hrps_env_tag *hrps_env = PRF_ENV_GET(HRPS, hrps);
    // Indicate the measurement
    struct hrps_meas_send_req *req = KE_MSG_ALLOC(HRPS_MEAS_SEND_REQ,
                                                  prf_src_task_get(&hrps_env->prf_env, conidx),
                                                  TASK_APP,
                                                  hrps_meas_send_req);

    memcpy(&req->meas_val, meas, sizeof(*meas));

    KE_MSG_SEND(req);

}

void app_hrps_set_initial_measurement_ind_cfg(bool enabled)
{
    measurement_ind_en = enabled;
}

#endif // (BLE_HR_SENSOR)

#endif // BLE_APP_PRESENT

/// @} APP
