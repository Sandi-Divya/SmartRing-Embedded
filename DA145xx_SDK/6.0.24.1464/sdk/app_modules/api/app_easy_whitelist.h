/**
 ****************************************************************************************
 * @addtogroup APP_Modules
 * @{
 * @addtogroup Easy_Whitelist Easy Whitelist
 * @brief Whitelist-related  functions API
 * @{
 *
 * @file app_easy_whitelist.h
 *
 * @brief Whitelist management API header file.
 *
 * Copyright (C) 2019-2025 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef _APP_EASY_WHITELIST_H_
#define _APP_EASY_WHITELIST_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdbool.h>
#include "gapm_task.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Whitelist operation type
typedef enum
{
    /// Get White List Size.
    APP_WLIST_OP_GET_SIZE           = GAPM_GET_WLIST_SIZE,

    /// Add devices in white list.
    APP_WLIST_OP_ADD_DEV            = GAPM_ADD_DEV_IN_WLIST,

    /// Remove devices form white list.
    APP_WLIST_OP_RMV_DEV            = GAPM_RMV_DEV_FRM_WLIST,

    /// Clear all devices from white list.
    APP_WLIST_OP_CLEAR              = GAPM_CLEAR_WLIST,
} app_wlist_op_t;

/// Whitelist address type
typedef enum
{
    /// Public address
    APP_WLIST_ADDR_PUBLIC            = ADDR_PUBLIC,

    /// Random address
    APP_WLIST_ADDR_RANDOM            = ADDR_RAND,

} app_wlist_addr_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Function for whitelist management.
 * @param[in] op        Command opcode
 * @param[in] addr      Peer device address to be added or removed
 * @param[in] type      Peer address type (public or random)
 * @return true if whitelist management operation is allowed to be done, else false
 ****************************************************************************************
 */
bool app_easy_manage_wlist(app_wlist_op_t op, struct bd_addr addr, app_wlist_addr_t type);

#endif // _APP_EASY_WHITELIST_H_

///@}
///@}
