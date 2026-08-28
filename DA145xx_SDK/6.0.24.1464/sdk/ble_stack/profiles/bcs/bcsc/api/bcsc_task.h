/**
 ****************************************************************************************
 *
 * @file bcsc_task.h
 *
 * @brief Header file - Body Composition Service Client Task Declaration.
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

#ifndef BCSC_TASK_H_
#define BCSC_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup BCSCTASK Center Client Task
 * @ingroup BCSC
 * @brief Body Composition Service Client Task
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_BCS_CLIENT)

#include "ke_task.h"
#include "gattc_task.h"
#include "bcsc.h"
#include "bcs_common.h"
#include "prf_types.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Possible states of the BCSC task
enum bcsc_state
{
    /// Free state
    BCSC_FREE,
    /// IDLE state
    BCSC_IDLE,
    /// Discovering Body Composition Service
    BCSC_DISCOVERING,
    /// Busy state
    BCSC_BUSY,
    /// Number of defined states
    BCSC_STATE_MAX
};

enum
{
    /// Start the Body Composition Service - at connection
    BCSC_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_BCSC),
    /// Confirm that cfg connection has finished with discovery results, or that normal cnx started
    BCSC_ENABLE_RSP,
    /// Generic message to read BCS characteristics
    BCSC_RD_CHAR_REQ,
    /// Generic message for read response
    BCSC_RD_CHAR_RSP,
    /// Message for configuration
    BCSC_BC_MEAS_CFG_IND_REQ,
    /// Configuration response
    BCSC_BC_MEAS_CFG_IND_RSP,
    /// Body Composition value send to APP
    BCSC_BC_MEAS_IND,
};

/// Codes for reading BCS characteristics
enum
{
    // Read Body Compostion Feature
    BCSC_RD_BC_FEAT,
    // Read Body Composition Measurment Client Cfg Descriptor
    BCSC_RD_BC_MEAS_CLI_CFG
};

/// Parameters of the @ref BCSC_ENABLE_REQ message
struct bcsc_enable_req
{
    /// Connection type
    uint8_t con_type;
    /// BCS handle values and characteristic properties
    struct bcs_content bcs;
};

/// Parameters of the @ref BCSC_ENABLE_CFM message
struct bcsc_enable_rsp
{
    /// Connection type
    uint8_t status;
    /// BCS handle values and characteristic properties
    struct bcs_content bcs;
};

/// Parameters of the @ref BCSC_RD_CHAR_REQ message
struct bcsc_rd_char_req
{
    /// Characteristic value code
    uint8_t char_code;
};

/// Parameters of the @ref BCSC_RD_CHAR_RSP message
struct bcsc_rd_char_rsp
{
    /// Requested Characteristic value code
    uint8_t char_code;
    /// Operation status
    uint8_t status;
    /// Value lenght
    uint8_t lenght;
    /// Read value
    uint8_t value[__ARRAY_EMPTY];
};

/// Parameters of the @ref BCSC_BC_MEAS_CFG_IND_REQ message
struct bcsc_bc_meas_cfg_ind_req
{
    /// Stop/notify/indicate value to configure into the peer characteristic
    uint16_t cfg_val;
};

/// Parameters of the @ref BCSC_BC_MEAS_CFG_IND_RSP message
struct bcsc_bc_meas_cfg_ind_rsp
{
    /// Status
    uint8_t status;
};

/// Parameters of the @ref BCSC_BC_MEAS_IND message
struct bcsc_bc_meas_ind
{
    uint16_t flags;
    /// Body Composition measurment value
    bcs_meas_t meas_val;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler bcsc_default_handler;
extern ke_state_t bcsc_state[BCSC_IDX_MAX];

#endif // (BLE_BCS_CLIENT)

/// @} BCSCTASK

#endif // BCSC_TASK_H_
