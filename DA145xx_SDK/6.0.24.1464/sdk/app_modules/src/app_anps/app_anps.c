/**
 ****************************************************************************************
 *
 * @file app_anps.c
 *
 * @brief Alert Notification Profile Server application.
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

#if (BLE_AN_SERVER)
#include "app.h"
#include "app_task.h"
#include "app_anps.h"
#include "app_prf_perm_types.h"
#include "prf_utils.h"
#include "user_callback_config.h"

#include "anps.h"
#include "anps_task.h"

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_anps_init(void)
{
    return;
}

void app_anps_create_db(void)
{
    struct anps_db_cfg* db_cfg;
    const struct att_incl_desc *incl = NULL;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct anps_db_cfg));

    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_ANPS);
    req->prf_task_id = TASK_ID_ANPS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    db_cfg = (struct anps_db_cfg* ) req->param;

    db_cfg->supp_new_alert_cat.cat_id_mask_0 = ANP_CAT_ID_SPL_ALERT_SUP | ANP_CAT_ID_EMAIL_SUP |
        ANP_CAT_ID_NEWS_SUP | ANP_CAT_ID_CALL_SUP | ANP_CAT_ID_MISSED_CALL_SUP |
        ANP_CAT_ID_SMS_MMS_SUP | ANP_CAT_ID_VOICE_MAIL_SUP | ANP_CAT_ID_SCHEDULE_SUP;
    db_cfg->supp_new_alert_cat.cat_id_mask_1 = ANP_CAT_ID_HIGH_PRTY_ALERT | ANP_CAT_ID_INST_MSG;

    db_cfg->supp_unread_alert_cat.cat_id_mask_0 = ANP_CAT_ID_SPL_ALERT_SUP | ANP_CAT_ID_EMAIL_SUP |
        ANP_CAT_ID_NEWS_SUP | ANP_CAT_ID_CALL_SUP | ANP_CAT_ID_MISSED_CALL_SUP |
        ANP_CAT_ID_SMS_MMS_SUP | ANP_CAT_ID_VOICE_MAIL_SUP | ANP_CAT_ID_SCHEDULE_SUP;
    db_cfg->supp_unread_alert_cat.cat_id_mask_1 = ANP_CAT_ID_HIGH_PRTY_ALERT | ANP_CAT_ID_INST_MSG;

    ke_msg_send(req);
}

void app_anps_enable(uint8_t conidx)
{
    struct anps_env_tag *anps_env = PRF_ENV_GET(ANPS, anps);

    struct anps_enable_rsp * rsp = KE_MSG_ALLOC(ANPS_ENABLE_RSP,
                                                 prf_src_task_get(&anps_env->prf_env, conidx),
                                                 TASK_APP,
                                                 anps_enable_rsp);
    rsp->status = 0x01;
    ke_msg_send(rsp);
}

void app_anps_ntf_alert_cmd(uint8_t conidx, const struct anps_ntf_alert_cmd *param)
{
    struct anps_env_tag *anps_env = PRF_ENV_GET(ANPS, anps);

    struct anps_ntf_alert_cmd *req = KE_MSG_ALLOC(ANPS_NTF_ALERT_CMD,
                                                  prf_src_task_get(&anps_env->prf_env, conidx),
                                                  TASK_APP,
                                                  anps_ntf_alert_cmd);
    
    req->operation = param->operation;
    req->value = param->value;

    ke_msg_send(req);
}

#endif // (BLE_AN_SERVER)

#endif // BLE_APP_PRESENT

/// @} APP
