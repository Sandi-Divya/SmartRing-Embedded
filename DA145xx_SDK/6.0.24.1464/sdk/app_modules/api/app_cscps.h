/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup CSCPS
 * @brief Cycling Speed and Cadence Profile Server Application API
 * @{
 *
 * @file app_cscps.h
 *
 * @brief Cycling Speed and Cadence Profile Server Application header.
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

#ifndef _APP_CSCPS_H_
#define _APP_CSCPS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_CSC_SENSOR)
#include "cscp_common.h"
#include "cscps_task.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// CSCP APP callbacks
struct app_cscps_cb
{
    /// Callback upon 'on_cscps_enable_rsp'
    void (*on_cscps_enable_rsp)(uint8_t conidx, uint8_t status);
    
    /// Callback upon 'on_cscps_meas_cfm'
    void (*on_cscps_meas_cfm)(uint8_t conidx, bool ind_en);

    /// Callback upon 'on_cscps_sc_cntl_pt_req_ind'
    void (*on_cscps_sc_cntl_pt_req_ind)(uint8_t conidx, struct cscps_sc_ctnl_pt_req_ind const *param);

    /// Callback upon 'on_cscps_cfg_ntfind_ind'
    void (*on_cscps_cfg_ntfind_ind)(uint8_t conidx, struct cscps_cfg_ntfind_ind const *param);
};


/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Cycling Speed and Cadence Service Application
 ****************************************************************************************
 */
void app_cscps_init(void);

/**
 ****************************************************************************************
 * @brief Add a Cycling Speed and Cadence Service instance in the DB
 ****************************************************************************************
 */
void app_cscps_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Cycling Speed and Cadence Profile Server.
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_cscps_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Cycling Speed and Cadence measurement
 * param[in] conidx          Connection index
 * param[in] meas            User config
 * @return   void
 ****************************************************************************************
 */
void app_cscps_ntf_csc_meas_req(uint8_t conidx, const struct cscp_csc_meas *meas);

/**
 ****************************************************************************************
 * @brief Cycling Speed and Cadence indication request
 * param[in] conidx          Connection index
 * param[in] param           User config
 * @return   void
 ****************************************************************************************
 */
void app_cscps_sc_ctnl_pt_req_ind(uint8_t conidx, const struct cscps_sc_ctnl_pt_req_ind *param);

/**
 ****************************************************************************************
 * @brief Send Cycling Speed and Cadence SC Control Point request confirmation
 * param[in] conidx          Connection index
 * param[in] param           User config
 * @return   void
 ****************************************************************************************
 */
void app_cscps_sc_ctnl_pt_cfm(uint8_t conidx, const struct cscps_sc_ctnl_pt_cfm *param);

/**
 ****************************************************************************************
 * @brief Send Cycling Speed and Cadence notification and indication config response
 * param[in] conidx          Connection index
 * param[in] param           User config
 * @return   void
 ****************************************************************************************
 */
void app_cscps_cfg_ntfind_ind(uint8_t conidx, const struct cscps_cfg_ntfind_ind *param);

/**
 ****************************************************************************************
 * @brief Set initial  Measurement notifications status restored when service is enabled.
 * param[in]  enabled  Whether notifications are enabled or not
 * @return    void
 ****************************************************************************************
 */
void app_cscps_set_initial_measurement_ind_cfg(bool enabled);

#endif // (BLE_CSC_SENSOR)

#endif // _APP_CSCPS_H_

///@}
///@}
///@}
