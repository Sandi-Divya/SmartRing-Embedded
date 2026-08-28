/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup CTSS
 * @brief Current Time Service Server Application API
 * @{
 *
 * @file app_ctss.h
 *
 * @brief Current Time Service Application entry point.
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
 *
 ****************************************************************************************
 */

#ifndef _APP_CTSS_H_
#define _APP_CTSS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_CTS_SERVER)

#include "cts_common.h"

/*
 * GLOBAL FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/// CTSS APP callbacks
struct app_ctss_cb
{
    /// Callback upon 'cur_time_read_req'
    void (*on_cur_time_read_req)(struct cts_curr_time *ct);

    /// Callback upon 'cur_time_write_req'
    uint8_t (*on_cur_time_write_req)(const struct cts_curr_time *ct);

    /// Callback upon 'cur_time_notified'
    void (*on_cur_time_notified)(uint8_t status);

    /// Callback upon 'loc_time_info_write_req'
    void (*on_loc_time_info_write_req)(const struct cts_loc_time_info *lt);

    /// Callback upon 'ref_time_info_read_req'
    void (*on_ref_time_info_read_req)(struct cts_ref_time_info *rt);
};

/**
 ****************************************************************************************
 * @brief Initialize Current Time Service Application
 ****************************************************************************************
 */
void app_ctss_init(void);

/**
 ****************************************************************************************
 * @brief Add a Current Time Service instance in the DB
 ****************************************************************************************
 */
void app_ctss_create_db(void);

/**
 ****************************************************************************************
 * @brief Notify Current Time Service clients
 ****************************************************************************************
 */
void app_ctss_notify_current_time(const struct cts_curr_time *curr_time);

#endif // (BLE_CTS_SERVER)

#endif // _APP_CTSS_H_

///@}
///@}
///@}
