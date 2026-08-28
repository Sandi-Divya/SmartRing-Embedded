/**
 ****************************************************************************************
 *
 * @file app_udss.c
 *
 * @brief User Data Service Server Application entry point
 *
 * Copyright (C) 2017-2025 Renesas Electronics Corporation and/or its affiliates.
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

#if (BLE_UDS_SERVER)

#include "app_api.h"
#include "app_udss.h"
#include "app_prf_perm_types.h"

#include "uds_common.h"
#include "udss_task.h"

/**
 ****************************************************************************************
 * User Data Service Server Application Functions
 ****************************************************************************************
 */

void app_udss_init(void)
{
    // nothing to do
}

void app_udss_create_db(void)
{
    struct udss_db_cfg* db_cfg;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct udss_db_cfg));

    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_UDSS);
    req->prf_task_id = TASK_ID_UDSS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    db_cfg = (struct udss_db_cfg* ) req->param;
    db_cfg->enabled_chars = APP_UDS_ENABLED_UDS_CHARS;
    db_cfg->local_char_update_enabled = APP_UDS_LOCAL_CHAR_UPDATE_ENABLED;

    KE_MSG_SEND(req);
}

void app_udss_cntl_point_operation_cfm(uint8_t conidx, const struct uds_ucp_rsp *rsp)
{
    struct udss_env_tag *udss_env = PRF_ENV_GET(UDSS, udss);

    struct udss_ucp_req_cfm * cfm = KE_MSG_ALLOC(UDSS_UCP_REQ_CFM,
                                                 prf_src_task_get(&udss_env->prf_env, conidx),
                                                 TASK_APP, udss_ucp_req_cfm);

    memcpy(&cfm->ucp_rsp, rsp, sizeof(struct uds_ucp_rsp));

    KE_MSG_SEND(cfm);
}

void app_udss_char_val_rsp(uint8_t conidx, uint8_t char_code, uint8_t length,
                           const uint8_t *value)
{
    struct udss_env_tag *udss_env = PRF_ENV_GET(UDSS, udss);
    struct udss_char_val *rsp = KE_MSG_ALLOC_DYN(UDSS_CHAR_VAL_RSP,
                                                 prf_src_task_get(&udss_env->prf_env, conidx),
                                                 TASK_APP, udss_char_val, length);

    if (length)
    {
        memcpy(rsp->value, value, length);
    }

    rsp->length = length;
    rsp->char_code = char_code;

    KE_MSG_SEND(rsp);
}

void app_udss_set_char_val_cfm(uint8_t conidx, uint8_t char_code, uint8_t status)
{
    struct udss_env_tag *udss_env = PRF_ENV_GET(UDSS, udss);
    struct udss_char_val_cfm *cfm = KE_MSG_ALLOC(UDSS_SET_CHAR_VAL_CFM,
                                                 prf_src_task_get(&udss_env->prf_env, conidx),
                                                 TASK_APP, udss_char_val_cfm);
    cfm->status = status;
    cfm->char_code = char_code;

    KE_MSG_SEND(cfm);
}

void app_udss_db_updated_notify(uint8_t conidx, uint32_t db_change_incr_val)
{
    struct udss_env_tag *udss_env = PRF_ENV_GET(UDSS, udss);
    struct udss_db_updated_ntf_req *ntf = KE_MSG_ALLOC(UDSS_DB_UPDATED_NTF_REQ,
                                                       prf_src_task_get(&udss_env->prf_env, conidx),
                                                       TASK_APP, udss_db_updated_ntf_req);
    ntf->db_change_incr_val = db_change_incr_val;

    KE_MSG_SEND(ntf);
}

#endif // (BLE_UDS_SERVER)

/// @} APP
