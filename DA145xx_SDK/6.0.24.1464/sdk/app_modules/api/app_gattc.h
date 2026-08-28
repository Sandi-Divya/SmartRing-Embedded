/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup GATTC
 * @brief Generic Attribute Profile Client Application API
 * @{
 *
 * @file app_gattc.h
 *
 * @brief Generic Attribute Profile Service Client Application entry point.
 *
 * Copyright (C) 2018-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef APP_GATTC_H_
#define APP_GATTC_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_GATT_CLIENT)

#include "gatt_client.h"

/*
 * GLOBAL FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/// GATTC APP callbacks
struct app_gattc_cb
{
    /// Callback upon 'enable'
    void (*on_gattc_enable)(uint8_t conidx, uint8_t status, const struct gatt_client_content *gatt);

    /// Callback upon 'rd_cfg_ind_rsp'
    void (*on_gattc_rd_cfg_ind_rsp)(uint8_t conidx, uint8_t status, bool enabled);

    /// Callback upon 'wr_cfg_ind_rsp'
    void (*on_gattc_wr_cfg_ind_rsp)(uint8_t conidx, uint8_t status);

    /// Callback upon 'svc_changed_ind'
    void (*on_gattc_svc_changed_ind)(uint8_t conidx, struct prf_svc *val);
};

/**
 ****************************************************************************************
 * @brief Initialize GATT Service Application
 ****************************************************************************************
 */
void app_gattc_init(void);

/**
 ****************************************************************************************
 * @brief Add a GATT Service instance
 ****************************************************************************************
 */
void app_gattc_create_task(void);

/**
 ****************************************************************************************
 * @brief Enable GATT Service client
 * @param[in] conidx            The connection id number
 ****************************************************************************************
 */
void app_gattc_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Write GATT service indication configuration
 * @param[in] conidx            The connection id number
 * @param[in] enable            Indication enable/disable
 ****************************************************************************************
 */
void app_gattc_write_ind_cfg(uint8_t conidx, bool enable);

/**
 ****************************************************************************************
 * @brief Read GATT service indicaion configuration value
 * @param[in] conidx            The connection id number
 ****************************************************************************************
 */
void app_gattc_read_ind_cfg(uint8_t conidx);

#endif // (BLE_GATT_CLIENT)

#endif // APP_GATTC_H_

///@}
///@}
///@}
