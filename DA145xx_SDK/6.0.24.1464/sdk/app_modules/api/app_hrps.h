/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup HRPS
 * @brief Heart Rate Profile Sensor Application API
 * @{
 *
 * @file app_hrps.h
 *
 * @brief Heart Rate Profile Sensor Application header.
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

#ifndef _APP_HRPS_H_
#define _APP_HRPS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_HR_SENSOR)
#include "hrp_common.h"
#include "hrps.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// HRPS APP callbacks
struct app_hrps_cb
{
    /// Callback upon 'hrps_ind_cfg_ind'
    void (*on_hrps_ind_cfg_ind)(uint8_t conidx, bool ind_en);
    
    /// Callback upon 'hrps_energy_reset_ind'
    void (*on_hrps_energy_exp_reset_ind)(uint8_t conidx);

    /// Callback upon 'hrps_send_measurement_cfm'
    void (*on_hrps_send_measurement_cfm)(uint8_t conidx, uint8_t status);
};


/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Heart Rate Service Application
 ****************************************************************************************
 */
void app_hrps_init(void);

/**
 ****************************************************************************************
 * @brief Add a Heart Rate Service instance in the DB
 ****************************************************************************************
 */
void app_hrps_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Heart Rate Profile Server.
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_hrps_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Heart Rate measurement
 * param[in] conidx          Connection index
 * param[in] meas            Heart Rate measurement
 ****************************************************************************************
 */
void app_hrps_send_measurement(uint8_t conidx, const struct hrs_hr_meas *meas);

/**
 ****************************************************************************************
 * @brief Set initial Measurement notifications status restored when service is enabled.
 * param[in] enabled     Whether notifications are enabled or not
 ****************************************************************************************
 */
void app_hrps_set_initial_measurement_ind_cfg(bool enabled);

#endif // (BLE_HR_SENSOR)

#endif // _APP_HRPS_H_

///@}
///@}
///@}
