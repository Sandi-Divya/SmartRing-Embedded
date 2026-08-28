/**
 ****************************************************************************************
 *
 * @file app_hogpd.c
 *
 * @brief HID Over GATT Profile Device Role application.
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

#if (BLE_HID_DEVICE)
#include "app.h"
#include "app_task.h"
#include "app_hogpd.h"
#include "app_prf_perm_types.h"
#include "prf_utils.h"
#include "user_callback_config.h"

#include "hogpd.h"
#include "hogpd_task.h"

// Define some default features when flag is not defined
#ifndef APP_HOGPD_FEATURES
#define APP_HOGPD_FEATURES  (HOGPD_CFG_KEYBOARD | HOGPD_CFG_MOUSE | \
                             HOGPD_CFG_PROTO_MODE | HOGPD_CFG_BOOT_KB_WR | \
                             HOGPD_CFG_BOOT_MOUSE_WR | HOGPD_CFG_REPORT_NTF_EN)
#endif

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_hogpd_init(void)
{
    return;
}

void app_hogpd_create_db(void)
{
    struct hogpd_db_cfg* db_cfg;
    const struct att_incl_desc *incl = NULL;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct hogpd_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_HOGPD);
    req->prf_task_id = TASK_ID_HOGPD;
    req->app_task = TASK_APP;
    req->start_hdl = 0;
    db_cfg = (struct hogpd_db_cfg* ) req->param;

    db_cfg->hids_nb = 1;
    db_cfg->cfg[0].svc_features = APP_HOGPD_FEATURES;

    db_cfg->cfg[0].report_nb = 3;
    db_cfg->cfg[0].report_char_cfg[0] = HOGPD_CFG_REPORT_IN | HOGPD_CFG_REPORT_WR;
    db_cfg->cfg[0].report_char_cfg[1] = HOGPD_CFG_REPORT_OUT | HOGPD_CFG_REPORT_WR;
    db_cfg->cfg[0].report_char_cfg[2] = HOGPD_CFG_REPORT_FEAT | HOGPD_CFG_REPORT_WR;

    db_cfg->cfg[0].report_id[0] = HOGPD_REPORT;
    db_cfg->cfg[0].report_id[1] = HOGPD_BOOT_KEYBOARD_INPUT_REPORT;
    db_cfg->cfg[0].report_id[2] = HOGPD_BOOT_MOUSE_INPUT_REPORT;

    db_cfg->cfg[0].hid_info.bcdHID = 0;
    db_cfg->cfg[0].hid_info.bCountryCode = 0;
    db_cfg->cfg[0].hid_info.flags = 0;

    db_cfg->cfg[0].ext_ref.inc_svc_hdl = 0;
    db_cfg->cfg[0].ext_ref.rep_ref_uuid = HOGPD_BOOT_KEYBOARD_INPUT_REPORT;

    // Send the message
    KE_MSG_SEND(req);
}

void app_hogpd_enable(uint8_t conidx)
{
    struct hogpd_env_tag *hogpd_env = PRF_ENV_GET(HOGPD, hogpd);
    // Allocate the message
    struct hogpd_enable_req * req = KE_MSG_ALLOC(HOGPD_ENABLE_REQ,
                                                 prf_src_task_get(&hogpd_env->prf_env, conidx),
                                                 TASK_APP,
                                                 hogpd_enable_req);

    req->conidx = conidx;
    req->ntf_cfg[0] = PRF_CLI_START_NTF;

    KE_MSG_SEND(req);
}

void app_hogpd_report_upd_req(uint8_t conidx, const struct hogpd_report_upd_req *data)
{
    struct hogpd_env_tag *hogpd_env = PRF_ENV_GET(HOGPD, hogpd);

    struct hogpd_report_upd_req *req = KE_MSG_ALLOC(HOGPD_REPORT_UPD_REQ,
                                                    prf_src_task_get(&hogpd_env->prf_env, conidx),
                                                    TASK_APP,
                                                    hogpd_report_upd_req);

    memcpy(req, data, sizeof(struct hogpd_report_upd_req));

    KE_MSG_SEND(req);
}

void app_hogpd_report_cfm(uint8_t conidx, const struct hogpd_report_cfm *rsp)
{
    struct hogpd_env_tag *hogpd_env = PRF_ENV_GET(HOGPD, hogpd);

    struct  hogpd_report_cfm *cfm = KE_MSG_ALLOC(HOGPD_REPORT_CFM,
                                                 prf_src_task_get(&hogpd_env->prf_env, conidx),
                                                 TASK_APP,
                                                 hogpd_report_cfm);

    memcpy(cfm, rsp, sizeof(struct hogpd_report_cfm));

    KE_MSG_SEND(cfm);
}

void app_hogpd_proto_mode_cfm(uint8_t conidx, const struct hogpd_proto_mode_cfm *rsp)
{
    struct hogpd_env_tag *hogpd_env = PRF_ENV_GET(HOGPD, hogpd);

    struct  hogpd_proto_mode_cfm *cfm = KE_MSG_ALLOC(HOGPD_PROTO_MODE_CFM,
                                                     prf_src_task_get(&hogpd_env->prf_env, conidx),
                                                     TASK_APP,
                                                     hogpd_proto_mode_cfm);

    memcpy(cfm, rsp, sizeof(struct hogpd_proto_mode_cfm));

    KE_MSG_SEND(cfm);
}

#endif // (BLE_HID_DEVICE)

#endif // BLE_APP_PRESENT

/// @} APP
