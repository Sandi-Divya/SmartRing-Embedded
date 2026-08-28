/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup TIPS
 * @brief Time Profile Server Application API
 * @{
 *
 * @file app_tips.h
 *
 * @brief Time Profile Server Application header.
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

#ifndef _APP_TIPS_H_
#define _APP_TIPS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_TIP_SERVER)
#include "tip_common.h"
#include "tips.h"
#include "tips_task.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// TIPS APP callbacks
struct app_tips_cb
{
    /// Callback upon 'tips_upd_curr_time_rsp'
    void (*on_tips_upd_curr_time_rsp)(uint8_t conidx, uint8_t status);
    
    /// Callback upon 'tips_rd_req_ind'
    void (*on_tips_rd_req_ind)(uint8_t conidx, uint8_t char_code);

    /// Callback upon 'tips_current_time_ccc_ind'
    void (*on_tips_current_time_ccc_ind)(uint8_t conidx, uint16_t cfg_val);

    /// Callback upon 'tips_time_upd_ctnl_pt_ind'
    void (*on_tips_time_upd_ctnl_pt_ind)(uint8_t conidx, uint8_t value);
};

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Time Profile Server Application
 ****************************************************************************************
 */
void app_tips_init(void);

/**
 ****************************************************************************************
 * @brief Add a Time Profile Server instance in the DB
 ****************************************************************************************
 */
void app_tips_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Time Profile Server
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_tips_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Current Time data
 * param[in] conidx          Connection index
 * param[in] *data           Current time request data
 ****************************************************************************************
 */
void app_tips_curr_time(uint8_t conidx, const struct tip_curr_time *time);

/**
 ****************************************************************************************
 * @brief Send Confirmation data
 * param[in] conidx          Connection index
 * param[in] *data           Confirmation request data
 ****************************************************************************************
 */
void app_tips_rd_cfm(uint8_t conidx, const struct tips_rd_cfm *data);

#endif // (BLE_TIP_SERVER)

#endif // _APP_TIPS_H_

///@}
///@}
///@}
