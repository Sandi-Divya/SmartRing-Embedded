/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup BLPS
 * @brief Blood Pressure Profile Service Server Application API
 * @{
 *
 * @file app_blps.h
 *
 * @brief Blood Pressure Profile Server Application header.
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

#ifndef _APP_BLPS_H_
#define _APP_BLPS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_BP_SENSOR)
#include "blp_common.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

struct app_blps_cb
{
    void (*on_blps_ind_cfg_ind)(uint8_t conidx, bool ind_en);
    void (*on_blps_send_measurement_cfm)(uint8_t conidx, uint8_t status);
};


/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Blood pressure Service Application
 ****************************************************************************************
 */
void app_blps_init(void);

/**
 ****************************************************************************************
 * @brief Add a Blood pressure Service instance in the DB
 ****************************************************************************************
 */
void app_blps_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Blood pressure Profile Server.
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_blps_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Blood pressure measurement
 * param[in] conidx          Connection index
 * param [in] flag_interm    Intermediary cuff pressure measurement or stable blood pressure measurement
 * param[in] meas            Blod pressure measurement
 * @return void
 ****************************************************************************************
 */
void app_blps_send_measurement(uint8_t conidx, uint8_t flag_interm, const struct bps_bp_meas *meas);

/**
 ****************************************************************************************
 * @brief Set initial Measurement notifications status restored when service is enabled.
 * param[in] enabled     Whether notifications are enabled or not
 * @return void
 ****************************************************************************************
 */
void app_blps_set_initial_measurement_ind_cfg(bool enabled);

#endif // (BLE_BP_SENSOR)

#endif // _APP_BLPS_H_

///@}
///@}
///@}
