/**
 ****************************************************************************************
 *
 * @file app_bmss.c
 *
 * @brief Bond Management Service Server Application entry point
 *
 * Copyright (C) 2015-2025 Renesas Electronics Corporation and/or its affiliates.
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

#include "rwble_config.h"

#if (BLE_BMS_SERVER)

#include "app_api.h"
#include "app_bmss.h"
#include "app_prf_perm_types.h"

#include "bms_common.h"
#include "bmss_task.h"
#include "user_profiles_config.h"


// Enable all features when feature flag is not set
#ifndef APP_BMS_FEATURES
#define APP_BMS_FEATURES BMS_FEAT_DEL_BOND_SUPPORTED | \
                         BMS_FEAT_DEL_ALL_BOND_SUPPORTED | \
                         BMS_FEAT_DEL_ALL_BOND_BUT_PEER_SUPPORTED
#endif

// Enable reliable writes if flag not defined elsewhere
#ifndef APP_BMS_RELIABLE_WRITES
#define APP_BMS_RELIABLE_WRITES (1)
#endif

 /**
 ****************************************************************************************
 * Bond Management Service Server Application Functions
 ****************************************************************************************
 */

void app_bmss_init(void)
{
    // nothing to do
}

void app_bmss_create_db(void)
{
    struct bmss_db_cfg* db_cfg;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct bmss_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_BMSS);
    req->prf_task_id = TASK_ID_BMSS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct bmss_db_cfg* ) req->param;
    db_cfg->features = APP_BMS_FEATURES;
    db_cfg->reliable_writes = APP_BMS_RELIABLE_WRITES;

    // Send the message
    KE_MSG_SEND(req);
}

void app_bmss_del_bond_cfm(uint8_t conidx, uint8_t status)
{
    struct bmss_env_tag *bmss_env = PRF_ENV_GET(BMSS, bmss);

    // Allocate the message
    struct bmss_del_bond_cfm * cfm = KE_MSG_ALLOC(BMSS_DEL_BOND_CFM,
                                            prf_src_task_get(&bmss_env->prf_env, conidx),
                                            TASK_APP, bmss_del_bond_cfm);

    // Fill in the parameter structure
    cfm->status = status;

    // Send the message
    KE_MSG_SEND(cfm);
}

#endif // (BLE_BMS_SERVER)

/// @} APP
