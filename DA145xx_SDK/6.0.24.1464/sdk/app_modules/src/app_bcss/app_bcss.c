/**
 ****************************************************************************************
 *
 * @file app_bcss.c
 *
 * @brief Body Composition Service Server application.
 *
 * Copyright (C) 2017-2023 Renesas Electronics Corporation and/or its affiliates.
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

#if (BLE_BCS_SERVER)
#include "app.h"
#include "app_task.h"
#include "app_bcss.h"
#include "app_prf_perm_types.h"

#include "bcs_common.h"
#include "bcss.h"
#include "bcss_task.h"
#include "arch.h"

// Define some default features when flag is not defined
#ifndef APP_BCSS_FEATURES
#define APP_BCSS_FEATURES       BCS_FEAT_TIME_STAMP | \
                                BCS_FEAT_MULTIPLE_USERS | \
                                BCS_FEAT_BASAL_METABOLISM | \
                                BCS_FEAT_MUSCLE_PERCENTAGE | \
                                BCS_FEAT_MUSCLE_MASS | \
                                BCS_FEAT_FAT_FREE_MASS | \
                                BCS_FEAT_SOFT_LEAN_MASS | \
                                BCS_FEAT_BODY_WATER_MASS | \
                                BCS_FEAT_IMPEDANCE | \
                                BCS_FEAT_WEIGHT | \
                                BCS_FEAT_HEIGHT | \
                                BCS_FEAT_MASS_RES_05_KG | \
                                BCS_FEAT_HEIGHT_RES_001_M
#endif // APP_BCSS_FEATURES

#ifndef APP_BCSS_IS_PRIMARY
#define APP_BCSS_IS_PRIMARY     (1)
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

void app_bcss_init(void)
{
    return;
}

void app_bcss_create_db(void)
{
    struct bcss_db_cfg* db_cfg;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct bcss_db_cfg));

    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_BCSS);
    req->prf_task_id = TASK_ID_BCSS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    db_cfg = (struct bcss_db_cfg* ) req->param;
    db_cfg->is_primary = APP_BCSS_IS_PRIMARY;
    db_cfg->features = APP_BCSS_FEATURES;

    KE_MSG_SEND(req);
}

void app_bcss_enable(uint8_t conidx)
{
    // Allocate the message
    struct bcss_env_tag *bcss_env = PRF_ENV_GET(BCSS, bcss);
    ASSERT_ERROR(bcss_env);
    struct bcss_enable_req * req = KE_MSG_ALLOC(BCSS_ENABLE_REQ,
                                                prf_src_task_get(&bcss_env->prf_env, conidx),
                                                TASK_APP,
                                                bcss_enable_req);

    req->meas_ind_en = measurement_ind_en ? GATT_CCC_START_IND : GATT_CCC_STOP_NTFIND;

    KE_MSG_SEND(req);
}

void app_bcss_send_measurement(uint8_t conidx, const bcs_meas_t *meas)
{

    struct bcss_env_tag *bcss_env = PRF_ENV_GET(BCSS, bcss);
    ASSERT_ERROR(bcss_env);
    struct bcss_meas_val_ind_req* ind = KE_MSG_ALLOC(BCSS_MEAS_VAL_IND_REQ,
                                                     prf_src_task_get(&bcss_env->prf_env, conidx),
                                                     TASK_APP,
                                                     bcss_meas_val_ind_req);
    ind->meas = *meas;
    KE_MSG_SEND(ind);

}

void app_bcss_set_initial_measurement_ind_cfg(bool enabled)
{
    measurement_ind_en = enabled;
}

#endif // (BLE_BCS_SERVER)

#endif // BLE_APP_PRESENT

/// @} APP
