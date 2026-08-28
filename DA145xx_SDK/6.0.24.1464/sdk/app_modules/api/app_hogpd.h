/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup HOGPD
 * @brief HID Over GATT Profile Device Role application API
 * @{
 *
 * @file app_hogpd.h
 *
 * @brief HID Over GATT Profile Device Role application header.
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

#ifndef _APP_HOGPD_H_
#define _APP_HOGPD_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_HID_DEVICE)
#include "hogp_common.h"
#include "hogpd.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// HOGPD APP callbacks
struct app_hogpd_cb
{
    /// Callback upon 'hogpd_report_upd_rsp'
    void (*on_hogpd_report_upd_rsp)(uint8_t conidx, uint8_t status);

    /// Callback upon 'hogpd_report_req_ind'
    void (*on_hogpd_report_req_ind)(uint8_t conidx, const struct hogpd_report_req_ind *req);

    /// Callback upon 'hogpd_proto_mode_req_ind'
    void (*on_hogpd_proto_mode_req_ind)(uint8_t conidx, const struct hogpd_proto_mode_req_ind *req);

    /// Callback upon 'hogpd_ntf_cfg_ind'
    void (*on_hogpd_ntf_cfg_ind)(uint8_t conidx, const struct hogpd_ntf_cfg_ind *req);

    /// Callback upon 'hogpd_ctnl_pt_ind'
    void (*on_hogpd_ctnl_pt_ind)(uint8_t conidx, const struct hogpd_ctnl_pt_ind *req);

};

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize HID Over GATT application
 ****************************************************************************************
 */
void app_hogpd_init(void);

/**
 ****************************************************************************************
 * @brief Add a HID Over GATT instance in data base
 ****************************************************************************************
 */
void app_hogpd_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable HID Over GATT device.
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_hogpd_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send HID Over GATT report update request to profile task
 * param[in] conidx          Connection index
 * param[in] *data           Request data
 ****************************************************************************************
 */
void app_hogpd_report_upd_req(uint8_t conidx, const struct hogpd_report_upd_req *data);

/**
 ****************************************************************************************
 * @brief Send HID Over GATT report confirmation to profile task
 * param[in] conidx          Connection index
 * param[in] *rsp            Confirmation data
 ****************************************************************************************
 */
void app_hogpd_report_cfm(uint8_t conidx, const struct hogpd_report_cfm *rsp);

/**
 ****************************************************************************************
 * @brief Send HID Over GATT protocol mode confirmation to profile task
 * param[in] conidx          Connection index
 * param[in] *rsp            Protocol mode confirmation data
 ****************************************************************************************
 */
void app_hogpd_proto_mode_cfm(uint8_t conidx, const struct hogpd_proto_mode_cfm *rsp);

#endif // (BLE_HID_DEVICE)

#endif // _APP_HOGPD_H_

///@}
///@}
///@}
