/**
 ****************************************************************************************
 *
 * @file app_findme.c
 *
 * @brief Findme locator and target application.
 *
 * Copyright (C) 2012-2023 Renesas Electronics Corporation and/or its affiliates.
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

#include "rwip_config.h"

#if (BLE_APP_PRESENT)

#include "app_findme.h"
#include "app_prf_perm_types.h"

#if (BLE_FINDME_LOCATOR)
#include "findl_task.h"
#include "app.h"
#endif

#if (BLE_FINDME_TARGET)
#include "gpio.h"
#include "findt_task.h"
#include "app.h"
#include "user_periph_setup.h"
#endif

/*
 * LOCAL VARIABLES DEFINITION
 ****************************************************************************************
 */

#if (BLE_FINDME_TARGET)
app_alert_state alert_state __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
#endif

#if (BLE_FINDME_TARGET)

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_findt_init()
{
    alert_state.port = GPIO_LED_PORT;
    alert_state.pin = GPIO_LED_PIN;
}

void app_findt_create_db(void)
{
    struct findt_db_cfg* db_cfg;
    
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd, 
                                                             sizeof(struct findt_db_cfg));
    
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_FINDT);
    req->prf_task_id = TASK_ID_FINDT;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct findt_db_cfg* ) req->param;
    db_cfg->dummy = 0;

    // Send the message
    KE_MSG_SEND(req);
}

void app_findt_alert_start(uint8_t lvl)
{
    alert_state.lvl = lvl;

    if (alert_state.lvl == FINDT_ALERT_MILD)
    {
        alert_state.blink_timeout = 150;
    }
    else
    {
        alert_state.blink_timeout = 50;
    }

    alert_state.blink_toggle = 1;
    GPIO_SetActive(alert_state.port, alert_state.pin);

    KE_TIMER_SET(APP_FINDT_TIMER, TASK_APP, alert_state.blink_timeout);
}

void app_findt_alert_stop(void)
{
    alert_state.lvl = FINDT_ALERT_NONE; // Clear alert level;

    alert_state.blink_timeout = 0;
    alert_state.blink_toggle = 0;

    GPIO_SetInactive(alert_state.port, alert_state.pin);

    KE_TIMER_CLEAR(APP_FINDT_TIMER, TASK_APP);
}

void default_findt_alert_ind_handler(uint8_t conidx, uint8_t lvl)
{
    if (lvl)
    {
        app_findt_alert_start(lvl);
    }
    else
    {
        app_findt_alert_stop();
    }
}
#endif // BLE_FINDME_TARGET

#if (BLE_FINDME_LOCATOR)
void app_findl_init(void)
{
}

void app_findl_create_task(void)
{
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd);
    
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_FINDL);
    req->prf_task_id = TASK_ID_FINDL;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Send the message
    KE_MSG_SEND(req);
}

void app_findl_enable(uint8_t conidx)
{
    // Allocate the message
    struct findl_enable_req *req = KE_MSG_ALLOC(FINDL_ENABLE_REQ,
                                                prf_get_task_from_id(TASK_ID_FINDL),
                                                TASK_APP,
                                                findl_enable_req);

    // Fill in the parameter structure
    req->con_type = PRF_CON_DISCOVERY;

    // Send the message
    KE_MSG_SEND(req);
}

void app_findl_set_alert(uint8_t lvl)
{
    struct findl_set_alert_req *req = KE_MSG_ALLOC(FINDL_SET_ALERT_REQ,
                                                   prf_get_task_from_id(TASK_ID_FINDL),
                                                   TASK_APP,
                                                   findl_set_alert_req);

    req->alert_lvl = lvl & (FINDL_ALERT_NONE | FINDL_ALERT_MILD | FINDL_ALERT_HIGH);

    // Send the message
    KE_MSG_SEND(req);
}
#endif // BLE_FINDME_LOCATOR

#endif // BLE_APP_PRESENT

/// @} APP
