/**
 ****************************************************************************************
 *
 * @file app_pasps.c
 *
 * @brief Phone Alert Status Profile Server application.
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

#if (BLE_PAS_SERVER)
#include "app.h"
#include "app_task.h"
#include "app_pasps.h"
#include "app_prf_perm_types.h"
#include "prf_utils.h"
#include "user_callback_config.h"

#include "pasps.h"
#include "pasps_task.h"

// Define some default features when flag is not defined
#ifndef APP_PASPS_ALERT_STATUS
#define APP_PASPS_ALERT_STATUS       PASP_RINGER_ACTIVE | PASP_VIBRATE_ACTIVE | PASP_DISP_ALERT_STATUS_ACTIVE
#endif

#ifndef APP_PASPS_RINGER_SETTINGS
#define APP_PASPS_RINGER_SETTINGS    PASP_RINGER_NORMAL
#endif

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */


/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_pasps_init(void)
{
    return;
}

void app_pasps_create_db(void)
{
    struct pasps_db_cfg* db_cfg;
    const struct att_incl_desc *incl = NULL;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct pasps_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_PASPS);
    req->prf_task_id = TASK_ID_PASPS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;
    db_cfg = (struct pasps_db_cfg* ) req->param;
    db_cfg->alert_status = APP_PASPS_ALERT_STATUS;
    db_cfg->ringer_setting = APP_PASPS_RINGER_SETTINGS;

    // Send the message
    KE_MSG_SEND(req);
}

void app_pasps_enable(uint8_t conidx)
{
    struct pasps_env_tag *pasps_env = PRF_ENV_GET(PASPS, pasps);
    // Allocate the message
    struct pasps_enable_req * req = KE_MSG_ALLOC(PASPS_ENABLE_REQ,
                                                prf_src_task_get(&pasps_env->prf_env, conidx),
                                                TASK_APP,
                                                pasps_enable_req);

    req->alert_status_ntf_cfg   = PASPS_FLAG_ALERT_STATUS_CFG;
    req->ringer_setting_ntf_cfg = PASPS_FLAG_RINGER_SETTING_CFG;

    KE_MSG_SEND(req);
}

void app_pasps_update_char_val(uint8_t conidx, const struct pasps_update_char_val_cmd *cmd)
{
    struct pasps_env_tag *pasps_env = PRF_ENV_GET(PASPS, pasps);

    struct pasps_update_char_val_cmd* req = KE_MSG_ALLOC(PASPS_UPDATE_CHAR_VAL_CMD,
                                                 prf_src_task_get(&pasps_env->prf_env, conidx),
                                                 TASK_APP, pasps_update_char_val_cmd);

    req->operation = cmd->operation;
    req->value = cmd->value;

    KE_MSG_SEND(req);
}

#endif // (BLE_PAS_SERVER)

#endif // BLE_APP_PRESENT

/// @} APP
