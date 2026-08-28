/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup BMSS
 * @brief Bond Management Service Server Application API
 * @{
 *
 * @file app_bmss.h
 *
 * @brief Bond Management Service Server Application header file
 *
 * Copyright (C) 2015-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef APP_BMSS_H_
#define APP_BMSS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_profiles_config.h"

#if BLE_BMS_SERVER

#include "bms_common.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// BMSS APP callbacks
struct app_bmss_cb
{
    /// Callback upon 'del_bond_req_ind'
    void (*on_bmss_del_bond_req_ind)(uint8_t con_idx,
                                        const struct bms_cntl_point_op *operation);
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes BMSS Apllication.
 ****************************************************************************************
 */
void app_bmss_init(void);

/**
 ****************************************************************************************
 * @brief Creates BMSS service database.
 ****************************************************************************************
 */
void app_bmss_create_db(void);

/**
 ****************************************************************************************
 * @brief Sends bond operation status to Bond Management Service.
 * @param[in] conidx Connection index
 * @param[in] status Operation status
 ****************************************************************************************
 */
void app_bmss_del_bond_cfm(uint8_t conidx, uint8_t status);

#endif // BLE_BMS_SERVER

#endif // APP_BMSS_H_

///@}
///@}
///@}
