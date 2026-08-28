/**
 ****************************************************************************************
 *
 * @file app_easy_whitelist.c
 *
 * @brief Whitelist management API implementation.
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

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include "app_easy_whitelist.h"
#include "gapm_task.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

bool app_easy_manage_wlist(app_wlist_op_t op,
                           struct bd_addr addr,
                           app_wlist_addr_t type)
{
    uint8_t state = ke_state_get(TASK_GAPM);

    if (state != GAPM_DEVICE_SETUP)
    {
        struct gapm_white_list_mgt_cmd *cmd = KE_MSG_ALLOC_DYN(GAPM_WHITE_LIST_MGT_CMD,
                                                               TASK_GAPM,
                                                               TASK_APP,
                                                               gapm_white_list_mgt_cmd,
                                                               sizeof(struct gap_bdaddr));

        cmd->operation = op;
        cmd->nb        = 1;
        memcpy(&(cmd->devices->addr), &addr, BD_ADDR_LEN * sizeof(uint8_t));
        cmd->devices->addr_type = type;

        // Send the message
        KE_MSG_SEND(cmd);

        return true;
    }
    return false;
}
/// @} APP
