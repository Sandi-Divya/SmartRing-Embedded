/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Profiles
 * @{
 * @addtogroup PASPS
 * @brief Phone Alert Status Profile Server Application API
 * @{
 *
 * @file app_pasps.h
 *
 * @brief Phone Alert Status Profile Server Application header.
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

#ifndef _APP_PASPS_H_
#define _APP_PASPS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_PAS_SERVER)
#include "pasp_common.h"
#include "pasps.h"
#include "pasps_task.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

/// PASPS APP callbacks
struct app_pasps_cb
{
    /// Callback upon 'pasps_written_char_val_ind'
    void (*on_pasps_written_char_val)(uint8_t conidx, const struct pasps_written_char_val_ind *req);
};

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Phone Alert Status Profile Server Application
 ****************************************************************************************
 */
void app_pasps_init(void);

/**
 ****************************************************************************************
 * @brief Add a Phone Alert Status Profile Server instance in the DB
 ****************************************************************************************
 */
void app_pasps_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Phone Alert Status Profile Server
 * param[in] conidx     Connection index
 ****************************************************************************************
 */
void app_pasps_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Phone Alert Status update value confirmation
 * param[in] conidx          Connection index
 * param[in] cmd             update command operation
 ****************************************************************************************
 */
void app_pasps_update_char_val(uint8_t conidx, const struct pasps_update_char_val_cmd *cmd);


#endif // (BLE_PAS_SERVER)

#endif // _APP_PASPS_H_

///@}
///@}
///@}
