/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup RSCPS
 * @brief Running Speed and Cadence Profile Sensor Application API
 * @{
 *
 * @file app_rscps.h
 *
 * @brief Running Speed and Cadence Profile Sensor Application header.
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

#ifndef _APP_RSCPS_H_
#define _APP_RSCPS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_RSC_SENSOR)
#include "rscp_common.h"
#include "rscps.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// RSCPS APP callbacks
struct app_rscps_cb
{
    /// Callback upon 'rscps_ind_cfg_ind'
    void (*on_rscps_ind_cfg_ind)(uint8_t conidx, uint8_t char_code, uint16_t ntf_cfg);

    /// Callback upon 'rscps_ntf_rsc_meas'
    void (*on_rscps_ntf_rsc_meas)(uint8_t conidx, uint8_t status);

    /// Callback upon 'rscps_sc_ctnl_pt_req_ind'
    void (*on_rscps_sc_ctnl_pt_req_ind)(uint8_t conidx, const struct rscps_sc_ctnl_pt_req_ind *req);
};

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Running Speed and Cadence Profile Sensor Application
 ****************************************************************************************
 */
void app_rscps_init(void);

/**
 ****************************************************************************************
 * @brief Add a Running Speed and Cadence Profile Sensor instance in the DB
 ****************************************************************************************
 */
void app_rscps_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Running Speed and Cadence Profile Sensor
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_rscps_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Navigation data
 * param[in] conidx          Connection index
 * param[in] meas            Running Speed and Cadence measurement
 ****************************************************************************************
 */
void app_rscps_send_meas(uint8_t conidx, const struct rscp_rsc_meas *meas);

/**
 ****************************************************************************************
 * @brief Send Control Point Operation confirmation
 * param[in] conidx          Connection index
 * param[in] rsp             Control Point operation response
 ****************************************************************************************
 */
void app_rscps_cntl_point_operation_cfm(uint8_t conidx, const struct rscp_sc_ctnl_pt_rsp *rsp);

#endif // (BLE_RSC_SENSOR)

#endif // _APP_RSCPS_H_

///@}
///@}
///@}
