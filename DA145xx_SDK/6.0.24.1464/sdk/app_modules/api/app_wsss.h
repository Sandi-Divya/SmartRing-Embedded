/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup WSSS
 * @brief Weight Scale Service Server Application API
 * @{
 *
 * @file app_wsss.h
 *
 * @brief Weight Scale Profile Server Application header.
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

#ifndef _APP_WSSS_H_
#define _APP_WSSS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_WSS_SERVER)
#include "wss_common.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// WSSS APP callbacks
struct app_wsss_cb
{
    /// Callback upon 'wsss_bcs_reference_req'
    const struct att_incl_desc *(*on_wsss_bcs_reference_req)(void);

    /// Callback upon 'wsss_ind_cfg_ind'
    void (*on_wsss_ind_cfg_ind)(uint8_t conidx, bool ind_en);

    /// Callback upon 'wsss_send_measurement_cfm'
    void (*on_wsss_send_measurement_cfm)(uint8_t conidx, uint8_t status);
};


/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Weight Scale Service Application
 ****************************************************************************************
 */
void app_wsss_init(void);

/**
 ****************************************************************************************
 * @brief Add a Weight Scale Service instance in the DB
 ****************************************************************************************
 */
void app_wsss_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Weight Scale Profile Server.
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_wsss_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Weight Scale measurement
 * param[in] conidx          Connection index
 * param[in] meas            Weight Scale measurement
 ****************************************************************************************
 */
void app_wsss_send_measurement(uint8_t conidx, const struct wss_wt_meas *meas);

/**
 ****************************************************************************************
 * @brief Set initial Measurement notifications status restored when service is enabled.
 * param[in] enabled     Whether notifications are enabled or not
 ****************************************************************************************
 */
void app_wsss_set_initial_measurement_ind_cfg(bool enabled);

#endif // (BLE_WSS_SERVER)

#endif // _APP_WSSS_H_

///@}
///@}
///@}
