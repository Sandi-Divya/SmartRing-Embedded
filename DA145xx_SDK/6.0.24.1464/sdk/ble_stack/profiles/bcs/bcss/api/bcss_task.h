/**
 ****************************************************************************************
 *
 * @file bcss_task.h
 *
 * @brief Header file - Body Composition Service Server Task.
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

#ifndef BCSS_TASK_H_
#define BCSS_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup BCSSTASK Body Composition Service Server
 * @ingroup BCSS
 * @brief Body Composition Service Server
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_BCS_SERVER)

#include "bcs_common.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Possible states of the DISS task
enum
{
    /// Idle state
    BCSS_IDLE,
    /// Connected state
    BCSS_BUSY,
    /// Number of defined states.
    BCSS_STATE_MAX
};

/// Messages for Body Composition Service Server
enum
{
    /// Start the Body Composition Service Server - at connection
    BCSS_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_BCSS),
    /// BCS enable response
    BCSS_ENABLE_RSP,
    /// Body Composition Measurement Value Indication Request
    BCSS_MEAS_VAL_IND_REQ,
    /// Body Composition Measurement Value Indication Response
    BCSS_MEAS_VAL_IND_RSP,
    /// Inform APP the if Notification Configuration has been changed
    BCSS_MEAS_VAL_IND_CFG_IND,
};

/*
 * APIs Structures
 ****************************************************************************************
 */

/// Parameters of the @ref BCSS_CREATE_DB_REQ message
struct bcss_db_cfg
{
    /// Type of service to instantiate (primary or secondary)
    uint8_t is_primary;
    /// Body Composition features
    uint32_t features;
};

/// Parameters of the @ref BCSS_ENABLE_REQ message
struct bcss_enable_req
{
    /// Body Composition Indication configuration
    uint16_t meas_ind_en;
};

/// Parameters of the @ref BCSS_ENABLE_RSP message
struct bcss_enable_rsp
{
    /// CCC initial configuration
    uint16_t init_cfg_ind;
    /// Operation status
    uint8_t status;
};

/// Parameters of the @ref BCSS_MEAS_VAL_IND_REQ message
struct bcss_meas_val_ind_req
{
    /// Measurement value
    bcs_meas_t meas;
};

/// Parameters of the @ref BCSS_MEAS_VAL_IND_RSP message
struct bcss_meas_val_ind_rsp
{
    /// Status
    uint8_t status;
};

/// Parameters of the @ref BCSS_MEAS_VAL_IND_CFG_IND message
struct bcss_meas_val_ind_cfg_ind
{
    /// Notification Configuration
    uint16_t ind_cfg;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler bcss_default_handler;

#endif // (BLE_BCS_SERVER)

/// @} BCSSTASK

#endif // BCSS_TASK_H_
