/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup LANS
 * @brief Location and Navigation Profile Application API
 * @{
 *
 * @file app_lans.h
 *
 * @brief Location and Navigation Profile Application header.
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

#ifndef _APP_LANS_H_
#define _APP_LANS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_LN_SENSOR)
#include "lan_common.h"
#include "lans.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// LANS APP callbacks
struct app_lans_cb
{
    /// Callback upon 'lans_ind_cfg_ind'
    void (*on_lans_ind_cfg_ind)(uint8_t conidx, uint8_t char_code, uint16_t ntf_cfg);

    /// Callback upon 'lans_ntf_loc_speed'
    void (*on_lans_ntf_loc_speed)(uint8_t conidx, uint8_t status);

    /// Callback upon 'lans_ntf_navigation'
    void (*on_lans_ntf_navigation)(uint8_t conidx, uint8_t status);

    /// Callback upon 'lans_upd_pos_q'
    void (*on_lans_upd_pos_q)(uint8_t conidx, uint8_t status);

    /// Callback upon 'lans_ln_ctnl_pt_req_ind'
    void (*on_lans_ln_ctnl_pt_req_ind)(uint8_t conidx, const struct lans_ln_ctnl_pt_req_ind *req);
};


/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Location and Navigation Service Application
 ****************************************************************************************
 */
void app_lans_init(void);

/**
 ****************************************************************************************
 * @brief Add a Location and Navigation Service instance in the DB
 ****************************************************************************************
 */
void app_lans_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Location and Navigation Profile Server.
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_lans_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Location and Navigation data
 * param[in] conidx          Connection index
 * param[in] *data           Location and Navigation request data
 ****************************************************************************************
 */
void app_lans_send_loc_speed_data(uint8_t conidx, const struct lanp_loc_speed *data);

/**
 ****************************************************************************************
 * @brief Send Position Quality data
 * param[in] conidx          Connection index
 * param[in] *data           Position Quality request data
 ****************************************************************************************
 */
void app_lans_send_pos_q_data(uint8_t conidx, const struct lanp_posq *data);

/**
 ****************************************************************************************
 * @brief Send Navigation data
 * param[in] conidx          Connection index
 * param[in] *data           Navigation request data
 ****************************************************************************************
 */
void app_lans_send_nav_data(uint8_t conidx, const struct lanp_navigation *data);

/**
 ****************************************************************************************
 * @brief Send Control Point data
 * param[in] conidx          Connection index
 * param[in] *data           Control Point request data
 ****************************************************************************************
 */
void app_lans_ln_ctnl_pt_ind(uint8_t conidx, const struct lan_ln_ctnl_pt_req *data);

/**
 ****************************************************************************************
 * @brief Send Control Point Operation confirmation
 * param[in] conidx          Connection index
 * param[in] *rsp            Control Point operation response
 ****************************************************************************************
 */
void app_lans_cntl_point_operation_cfm(uint8_t conidx, const struct lanp_ln_ctnl_pt_rsp *rsp);

#endif // (BLE_LN_SENSOR)

#endif // _APP_LANS_H_

///@}
///@}
///@}
