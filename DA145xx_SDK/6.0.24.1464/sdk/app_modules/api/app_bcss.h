/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup BCSS
 * @brief Body Composition Service Server Application API
 * @{
 *
 * @file app_bcss.h
 *
 * @brief Header file - Body Composition Service Server Application Layer.
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
 ****************************************************************************************
 */

#ifndef APP_BCSS_H_
#define APP_BCSS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_BCS_SERVER)
#include "bcs_common.h"
#include "attm.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// APP BCSS callbacks
struct app_bcss_cb
{
    /// Service enabled message callback
    void (*on_bcss_enable_rsp)(uint8_t conidx, uint8_t status, uint16_t init_cfg_ind);

    /// Measurement send response message callback
    void (*on_bcss_meas_val_ind_rsp)(uint8_t conidx, uint8_t status);

    /// Client Characteristic Configuration Descriptor changed message callback
    void (*on_bcss_ind_cfg_ind)(uint8_t conidx, bool status);
};


/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Body Composition Service Application
 ****************************************************************************************
 */
void app_bcss_init(void);

/**
 ****************************************************************************************
 * @brief Add a Body Composition Service instance in the DB
 ****************************************************************************************
 */
void app_bcss_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Body Composition Service Server.
 * param[in] conidx     Connection handle
 ****************************************************************************************
 */
void app_bcss_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Body Composition measurement
 * param[in] connhdl          Connection handle
 * param[in] scan_refresh_val Scan Refresh value
 ****************************************************************************************
 */
void app_bcss_send_measurement(uint8_t conidx, const bcs_meas_t *meas);

/**
 ****************************************************************************************
 * @brief Set initial Measurement notifications status restored when service is enabled.
 * param[in] enabled     Whether notifications are enabled or not
 ****************************************************************************************
 */
void app_bcss_set_initial_measurement_ind_cfg(bool enabled);

#endif // (BLE_BCS_SERVER)

#endif

///@}
///@}
///@}
