/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup HTPT
 * @brief Health Thermometer Service Server Application API
 * @{
 *
 * @file app_htpt.h
 *
 * @brief Health Thermometer Profile Server Application header.
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

#ifndef _APP_HTPT_H_
#define _APP_HTPT_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_HT_THERMOM)
#include "htp_common.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// HTPT APP callbacks
struct app_htpt_cb
{
    /// Callback upon 'htpt_ind_cfg_ind'
    void (*on_htpt_ind_cfg_ind)(uint8_t conidx, bool ind_en);

    /// Callback upon 'htpt_send_measurement_cfm'
    void (*on_htpt_send_measurement_cfm)(uint8_t conidx, uint8_t status);

    /// Callback upon 'htpt_send_intv_chg_cfm'
    void (*on_htpt_send_intv_chg_cfm)(uint8_t conidx, uint16_t intv);
};


/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Health Thermometer Service Application
 ****************************************************************************************
 */
void app_htpt_init(void);

/**
 ****************************************************************************************
 * @brief Add a Health Thermometer Service instance in the DB
 ****************************************************************************************
 */
void app_htpt_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Health Thermometer Profile Server.
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_htpt_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Health Thermometer measurement
 * param[in] conidx          Connection index
 * param[in] stable_meas     Stable or intermediary type of temperature
 *                           (true for stable measurement, false for intermediate)
 * param[in] meas            Temperature measurement
 * @return   void
 ****************************************************************************************
 */
void app_htpt_send_measurement(uint8_t conidx, uint8_t stable_meas, const struct htp_temp_meas *meas);

/**
 ****************************************************************************************
 * @brief Send Health Thermometer measurement interval
 * param[in] conidx          Connection index
 * param[in] meas_intv       Temperature measurement interval
 * @return   void
 ****************************************************************************************
 */
void app_htpt_send_measurement_interval(uint8_t conidx, uint16_t meas_intv);

/**
 ****************************************************************************************
 * @brief Send Health Thermometer measurement interval change request confirmation
 * param[in] conidx          Connection index
 * param[in] status          Temperature measurement interval change status to be sent
 * @return   void
 ****************************************************************************************
 */
void app_htpt_send_measurement_interval_chg_cfm(uint8_t conidx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Set initial  Measurement notifications status restored when service is enabled.
 * param[in]  enabled  Whether notifications are enabled or not
 * @return    void
 ****************************************************************************************
 */
void app_htpt_set_initial_measurement_ind_cfg(bool enabled);

#endif // (BLE_HT_THERMOM)

#endif // _APP_HTPT_H_

///@}
///@}
///@}
