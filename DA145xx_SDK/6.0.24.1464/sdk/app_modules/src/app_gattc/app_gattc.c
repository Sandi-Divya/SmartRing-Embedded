/**
 ****************************************************************************************
 *
 * @file app_gattc.c
 *
 * @brief Generic Attribute Profile Service Application entry point
 *
 * Copyright (C) 2018-2023 Renesas Electronics Corporation and/or its affiliates.
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

#if (BLE_GATT_CLIENT)

#include "app_gattc.h"
#include "app.h"
#include "app_task.h"
#include "gatt_client_task.h"
#include "app_prf_perm_types.h"
#include "user_profiles_config.h"
#include "arch.h"

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_gattc_init(void)
{
    // Nothing to do
}

void app_gattc_create_task(void)
{
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC(GAPM_PROFILE_TASK_ADD_CMD,
                                                         TASK_GAPM,
                                                         TASK_APP,
                                                         gapm_profile_task_add_cmd);

    // Fill the message
    req->operation   = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl     = get_user_prf_srv_perm(TASK_ID_GATT_CLIENT);
    req->prf_task_id = TASK_ID_GATT_CLIENT;
    req->app_task    = TASK_APP;
    req->start_hdl   = 0;

    // Send the message
    KE_MSG_SEND(req);
}

void app_gattc_enable(uint8_t conidx)
{
    struct gatt_client_env_tag *gatt_env = PRF_ENV_GET(GATT_CLIENT, gatt_client);
    ASSERT_ERROR(gatt_env);
    struct gatt_client_enable_req *req = KE_MSG_ALLOC(GATT_CLIENT_ENABLE_REQ,
                                                      prf_src_task_get(&gatt_env->prf_env, conidx),
                                                      TASK_APP,
                                                      gatt_client_enable_req);

    // Provide the connection type
    req->con_type = PRF_CON_DISCOVERY;

    // Send the message
    KE_MSG_SEND(req);
}

void app_gattc_read_ind_cfg(uint8_t conidx)
{
    struct gatt_client_env_tag *gatt_env = PRF_ENV_GET(GATT_CLIENT, gatt_client);
    ASSERT_ERROR(gatt_env);

    struct gatt_client_rd_cfg_ind_req *req = KE_MSG_ALLOC(GATT_CLIENT_RD_CFG_IND_REQ,
                                                      prf_src_task_get(&gatt_env->prf_env, conidx),
                                                      TASK_APP,
                                                      gatt_client_rd_cfg_ind_req);

    KE_MSG_SEND(req);
}

void app_gattc_write_ind_cfg(uint8_t conidx, bool enable)
{
    struct gatt_client_env_tag *gatt_env = PRF_ENV_GET(GATT_CLIENT, gatt_client);
    ASSERT_ERROR(gatt_env);

    struct gatt_client_wr_cfg_ind_req *req = KE_MSG_ALLOC(GATT_CLIENT_WR_CFG_IND_REQ,
                                                      prf_src_task_get(&gatt_env->prf_env, conidx),
                                                      TASK_APP,
                                                      gatt_client_wr_cfg_ind_req);

    req->cfg_val = (enable) ? PRF_CLI_START_IND : PRF_CLI_STOP_NTFIND;
    KE_MSG_SEND(req);
}

#endif // (BLE_GATT_CLIENT)

#endif // BLE_APP_PRESENT

/// @} APP
