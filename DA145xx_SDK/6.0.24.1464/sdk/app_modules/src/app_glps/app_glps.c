/**
 ****************************************************************************************
 *
 * @file app_glps.c
 *
 * @brief Glucose Profile Sensor
 *
 * Copyright (C) 2012-2024 Renesas Electronics Corporation and/or its affiliates.
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

#if (BLE_GL_SENSOR)
#include "app.h"
#include "app_task.h"
#include "app_glps.h"
#include "app_prf_perm_types.h"
#include "prf_utils.h"
#include "user_callback_config.h"
#include "arch_console.h"

#include "glps.h"
#include "glps_task.h"

// Define some default features when flag is not defined

#ifndef APP_GLPS_CTX_SUPPORTED
#define APP_GLPS_CTX_SUPPORTED          GLP_CTX_CRBH_ID_AND_CRBH_PRES | GLP_CTX_MEAL_PRES | \
                                        GLP_CTX_TESTER_HEALTH_PRES | GLP_CTX_EXE_DUR_AND_EXE_INTENS_PRES | \
                                        GLP_CTX_MEDIC_ID_AND_MEDIC_PRES | GLP_CTX_MEDIC_VAL_UNITS_KG | \
                                        GLP_CTX_HBA1C_PRES | GLP_CTX_EXTD_F_PRES
#endif

#ifndef APP_GLPS_FEATURES
#define APP_GLPS_FEATURES               GLP_FET_LOW_BAT_DET_DUR_MEAS_SUPP | GLP_FET_SENS_MFNC_DET_SUPP | \
                                        GLP_FET_SENS_SPL_SIZE_SUPP | GLP_FET_SENS_STRIP_INSERT_ERR_DET_SUPP | \
                                        GLP_FET_SENS_STRIP_TYPE_ERR_DET_SUPP | GLP_FET_SENS_RES_HIGH_LOW_DET_SUPP | \
                                        GLP_FET_SENS_TEMP_HIGH_LOW_DET_SUPP | GLP_FET_SENS_RD_INT_DET_SUPP | \
                                        GLP_FET_GEN_DEV_FLT_SUPP | GLP_FET_TIME_FLT_SUPP | GLP_FET_MUL_BOND_SUPP
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

void app_glps_init(void)
{
    return;
}

void app_glps_create_db(void)
{
    arch_printf("app_glps_create_db started\n\r");

    struct glps_db_cfg* db_cfg;
    const struct att_incl_desc *incl = NULL;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct glps_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_GLPS);
    req->prf_task_id = TASK_ID_GLPS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    db_cfg = (struct glps_db_cfg* ) req->param;
    db_cfg->features = APP_GLPS_FEATURES;
    db_cfg->meas_ctx_supported = APP_GLPS_CTX_SUPPORTED;

    ke_msg_send(req);
}

void app_glps_enable(uint8_t conidx)
{
    struct glps_env_tag *glps_env = PRF_ENV_GET(GLPS, glps);
    // Allocate the message
    struct glps_enable_req * req = KE_MSG_ALLOC(GLPS_ENABLE_REQ,
                                                prf_src_task_get(&glps_env->prf_env, conidx),
                                                TASK_APP,
                                                glps_enable_req);

    req->evt_cfg = measurement_ind_en ? GATT_CCC_START_IND : GATT_CCC_STOP_NTFIND;

    ke_msg_send(req);
}

void app_glps_send_measurement(uint8_t conidx, const struct glp_meas *meas)
{
    struct glps_env_tag *glps_env = PRF_ENV_GET(GLPS, glps);
    // Indicate the measurement
    struct glps_send_meas_without_ctx_cmd *req = KE_MSG_ALLOC(GLPS_SEND_MEAS_WITHOUT_CTX_CMD,
                                                  prf_src_task_get(&glps_env->prf_env, conidx),
                                                  TASK_APP,
                                                  glps_send_meas_without_ctx_cmd);

    req->seq_num = 1;
    memcpy(&req->meas, meas, sizeof(*meas));

    ke_msg_send(req);
}

void app_glps_set_initial_measurement_ind_cfg(bool enabled)
{
    measurement_ind_en = enabled;
}

#endif // (BLE_GL_SENSOR)

#endif // BLE_APP_PRESENT

/// @} APP
