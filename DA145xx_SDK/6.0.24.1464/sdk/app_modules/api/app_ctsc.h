/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup CTSC
 * @brief Current Time Service Client Application API
 * @{
 *
 * @file app_ctsc.h
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

#ifndef _APP_CTSC_H_
#define _APP_CTSC_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_CTS_CLIENT)

#include "cts_common.h"
#include "ctsc.h"

/*
 * GLOBAL FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/// CTSC APP callbacks
struct app_ctsc_cb
{
    /// Callback upon 'connect'
    void (*on_connect)(uint8_t conidx, const struct ctsc_cts_content *cts);

    /// Callback upon 'ct_read'
    void (*on_ct_read)(uint8_t conidx, uint8_t status, const struct cts_curr_time *ct, bool notification);

    /// Callback upon 'rti_read'
    void (*on_rti_read)(uint8_t conidx, uint8_t status, const struct cts_ref_time_info *ct);

    /// Callback upon 'lti_read'
    void (*on_lti_read)(uint8_t conidx, uint8_t status, const struct cts_loc_time_info *lt);

    /// Callback upon 'desc_read'
    void (*on_desc_read)(uint8_t conidx, uint8_t status, uint16_t value);

    /// Callback upon 'ct_write'
    void (*on_ct_write)(uint8_t conidx, uint8_t status);

    /// Callback upon 'lti_write'
    void (*on_lti_write)(uint8_t conidx, uint8_t status);

    /// Callback upon 'desc_write'
    void (*on_desc_write)(uint8_t conidx, uint8_t status);
};

/**
 ****************************************************************************************
 * @brief Initialize Current Time Service Application
 ****************************************************************************************
 */
void app_ctsc_init(void);

/**
 ****************************************************************************************
 * @brief Add a Current Time Service instance in the DB
 ****************************************************************************************
 */
void app_ctsc_create_task(void);

/**
 ****************************************************************************************
 * @brief Enable Current Time Service client profile.
 * @param[in] conidx            The connection id number
 ****************************************************************************************
 */
void app_ctsc_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Read current time from peer
 ****************************************************************************************
 */
void app_ctsc_read_ct(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Read Local Time information from peer
 ****************************************************************************************
 */
void app_ctsc_read_lti(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Read Reference Time information from peer
 ****************************************************************************************
 */
void app_ctsc_read_rti(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Read Current Time conjuration descriptor from peer
 ****************************************************************************************
 */
void app_ctsc_read_desc(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Write current time to peer
 ****************************************************************************************
 */
void app_ctsc_write_ct(uint8_t conidx, const struct cts_curr_time *ct);

/**
 ****************************************************************************************
 * @brief Write local time information to peer
 ****************************************************************************************
 */
void app_ctsc_write_lti(uint8_t conidx, const struct cts_loc_time_info *lti);

/**
 ****************************************************************************************
 * @brief Write Current Time configuration descriptor to peer
 ****************************************************************************************
 */
void app_ctsc_write_desc(uint8_t conidx, uint16_t value);

#endif // (BLE_CTS_CLIENT)

#endif // _APP_CTSC_H_

///@}
///@}
///@}
