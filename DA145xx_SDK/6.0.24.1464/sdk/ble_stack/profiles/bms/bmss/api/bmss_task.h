/**
 ****************************************************************************************
 *
 * @file bmss_task.h
 *
 * @brief Header file - Bond Management Service Server Role Task Declaration.
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

#ifndef BMSS_TASK_H_
#define BMSS_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup BMSSTASK Task
 * @ingroup BMSS
 * @brief Bond Management 'Profile' Task.
 *
 * The BMSS_TASK is responsible for handling the messages coming in and out of the
 * @ref BMSS block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
*/

#if (BLE_BMS_SERVER)

#include "ke_task.h"
#include "bms_common.h"
#include "bmss.h"

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Possible states of the BMSS task
enum
{
    /// Idle state
    BMSS_IDLE,
    /// Busy state
    BMSS_BUSY,
    /// Number of defined states
    BMSS_STATE_MAX,
};

/// Messages for Bond Management Server
enum
{
    /// Error Indication
    BMSS_ERROR_IND = KE_FIRST_MSG(TASK_ID_BMSS),
    /// Delete Bond Request Indication
    BMSS_DEL_BOND_REQ_IND,
    /// Delete Bond Write Response
    BMSS_DEL_BOND_CFM,
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

///Parameters of the Bond Management service database
struct bmss_db_cfg
{
    /// Features of BMS instance
    uint32_t features;
    /// Extended properties - reliable writes (supported or not)
    uint8_t reliable_writes;
};

/// Parameters of the @ref BMSS_DEL_BOND_REQ_IND message - dynamically allocated
struct bmss_del_bond_req_ind
{
    /// Operation code and operand
    struct bms_cntl_point_op operation;
};

/// Parameters of the @ref BMSS_DEL_BOND_CFM message
struct bmss_del_bond_cfm
{
    /// Bond deletion result status
    uint8_t status;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler bmss_state_handler[BMSS_STATE_MAX];
extern const struct ke_state_handler bmss_default_handler;

#endif // BLE_BMS_SERVER

/// @} BMSSTASK

#endif // BMSS_TASK_H_
