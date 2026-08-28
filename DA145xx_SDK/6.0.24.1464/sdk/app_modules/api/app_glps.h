/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup GLPS
 * @brief Glucose Profile Service Server Application API
 * @{
 *
 * @file app_glps.h
 *
 * @brief Glucose Profile Server Application header.
 *
 * Copyright (C) 2017-2023 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _APP_GLPS_H_
#define _APP_GLPS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_GL_SENSOR)
#include "glp_common.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// GLPS APP callbacks
struct app_glps_cb
{
    /// Callback upon 'glps_bcs_reference_req'
    const struct att_incl_desc *(*on_glps_bcs_reference_req)(void);

    /// Callback upon 'glps_ind_cfg_ind'
    void (*on_glps_ind_cfg_ind)(uint8_t conidx, bool ind_en);

    /// Callback upon 'glps_send_measurement_cfm'
    void (*on_glps_send_measurement_cfm)(uint8_t conidx, uint8_t status);
};


/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Glucose Profile Service Application
 ****************************************************************************************
 */
void app_glps_init(void);

/**
 ****************************************************************************************
 * @brief Add a Glucose Profile Service instance in the DB
 ****************************************************************************************
 */
void app_glps_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Glucose Profile Profile Server.
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_glps_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Glucose Profile measurement
 * param[in] conidx          Connection index
 * param[in] meas            Glucose measurement
 ****************************************************************************************
 */
void app_glps_send_measurement(uint8_t conidx, const struct glp_meas *meas);

/**
 ****************************************************************************************
 * @brief Set initial Measurement notifications status restored when service is enabled.
 * param[in] enabled     Whether notifications are enabled or not
 ****************************************************************************************
 */
void app_glps_set_initial_measurement_ind_cfg(bool enabled);

#endif // (BLE_GL_SENSOR)

#endif // _APP_GLPS_H_

///@}
///@}
///@}
