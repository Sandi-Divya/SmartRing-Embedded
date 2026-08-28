/**
 ****************************************************************************************
 *
 * @file app_proxr.c
 *
 * @brief Proximity Reporter application.
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

#include "rwble_config.h"

#if (BLE_APP_PRESENT)

#if (BLE_PROX_REPORTER)
#include "app_api.h"                // application task definitions
#include "proxr_task.h"              // proximity functions
#include "gpio.h"
#include "wkupct_quadec.h"
#include "app_proxr.h"
#include "app_prf_perm_types.h"
#include "user_periph_setup.h"
#include "ke_timer.h"

//application alert state structure
app_alert_state alert_state __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_proxr_init()
{
    alert_state.port = GPIO_ALERT_LED_PORT;
    alert_state.pin = GPIO_ALERT_LED_PIN;
    alert_state.ll_alert_lvl = PROXR_ALERT_HIGH; // Link Loss default Alert Level
    alert_state.txp_lvl = 0x00;                  // TX power level indicator
}

void app_proxr_create_db(void)
{
    struct proxr_db_cfg* db_cfg;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct proxr_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_PROXR);
    req->prf_task_id = TASK_ID_PROXR;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct proxr_db_cfg* ) req->param;
    db_cfg->features = PROXR_IAS_TXPS_SUP;

    // Send the message
    KE_MSG_SEND(req);
}

void app_proxr_alert_start(uint8_t lvl)
{
    alert_state.lvl = lvl;

    if (alert_state.lvl == PROXR_ALERT_MILD)
    {
        alert_state.blink_timeout = 150;
    }
    else
    {
        alert_state.blink_timeout = 50;
    }

    alert_state.blink_toggle = 1;

    GPIO_SetActive( alert_state.port, alert_state.pin);

    KE_TIMER_SET(APP_PROXR_TIMER, TASK_APP, alert_state.blink_timeout);
}

void app_proxr_alert_stop(void)
{
    alert_state.lvl = PROXR_ALERT_NONE;

    alert_state.blink_timeout = 0;
    alert_state.blink_toggle = 0;

    GPIO_SetInactive(alert_state.port, alert_state.pin);

    KE_TIMER_CLEAR(APP_PROXR_TIMER, TASK_APP);
}

void app_proxr_port_reinit(GPIO_PORT port, GPIO_PIN pin)
{
    alert_state.port = port;
    alert_state.pin = pin;

    if (alert_state.blink_toggle == 1)
    {
        GPIO_SetActive(alert_state.port, alert_state.pin);
    }
    else
    {
        GPIO_SetInactive(alert_state.port, alert_state.pin);
    }
}

/**
 ****************************************************************************************
 * @brief Update proximity application alert.
 * @param[in] lvl     Alert level: PROXR_ALERT_NONE, PROXR_ALERT_MILD, PROXR_ALERT_HIGH
 ****************************************************************************************
 */
static void alert_update(uint8_t alert_lvl)
{
    if (alert_lvl)
    {
        app_proxr_alert_start(alert_lvl);
    }
    else
    {
        app_proxr_alert_stop();
    }
}

void default_proxr_alert_ind_handler(uint8_t conidx, uint8_t alert_lvl, uint8_t char_code)
{
    alert_update(alert_lvl);
}

#endif // BLE_PROX_REPORTER

#endif // BLE_APP_PRESENT

/// @} APP
