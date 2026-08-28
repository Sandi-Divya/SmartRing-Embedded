/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup ANPS
 * @brief Alert Notification Profile Server Application API
 * @{
 *
 * @file app_anps.h
 *
 * @brief Alert Notification Profile Server Application header.
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

#ifndef _APP_ANPS_H_
#define _APP_ANPS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_AN_SERVER)
#include "anp_common.h"
#include "anps_task.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// ANP APP callbacks
struct app_anps_cb
{
    /// Callback upon 'anps_enable_rsp'
    void (*on_anps_enable_rsp)(const struct anps_enable_rsp *param);

    /// Callback upon 'anps_ntf_immediate_req_ind'
    void (*on_anps_ntf_immediate_req_ind)(uint8_t conidx, const struct anps_ntf_immediate_req_ind *param);

    /// Callback upon 'anps_ntf_status_update_ind'
    void (*on_anps_ntf_status_update_ind)(uint8_t conidx, const struct anps_ntf_status_update_ind *param);
};


/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Alert Notification Profile Server Application
 ****************************************************************************************
 */
void app_anps_init(void);

/**
 ****************************************************************************************
 * @brief Add Alert Notification Profile Server instance in the DB
 ****************************************************************************************
 */
void app_anps_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Alert Notification Profile Server.
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_anps_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Alert Notification
 * param[in] conidx          Connection index
 * param[in] param           User params
 * @return   void
 ****************************************************************************************
 */
void app_anps_ntf_alert_cmd(uint8_t conidx, const struct anps_ntf_alert_cmd *param);

#endif // (BLE_AN_SERVER)

#endif // _APP_ANPS_H_

///@}
///@}
///@}
