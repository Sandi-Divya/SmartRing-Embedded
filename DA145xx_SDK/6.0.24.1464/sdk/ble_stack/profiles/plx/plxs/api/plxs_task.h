/**
 ****************************************************************************************
 *
 * @file plxs_task.h
 *
 * @brief Header file - Pulse Oximeter Service Server Role Task Declaration.
 *
 * Copyright (C) 2015-2024 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef PLXS_TASK_H_
#define PLXS_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup PLXSTASK Task
 * @ingroup PLXS
 * @brief Pulse Oximeter Profile Server Task
 *
 * The PLXSTASK is responsible for handling the messages coming in and out of the
 * @ref PLXS reporter block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#if (BLE_PLX_SERVER)
#include <stdint.h>
#include "ke_task.h"
#include "plxs.h"
#include "prf_types.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define PLXS_IDX_MAX        (1)

/// Possible states of the PLXS task
enum plxs_state
{
    /// Idle state
    PLXS_IDLE,
    /// Busy state
    PLXS_BUSY,

    /// Number of defined states.
    PLXS_STATE_MAX
};

/// Messages for Pulse Oximeter Profile Server
enum plxs_msg_id
{
    /// Start the Pulse Oximeter Profile - at connection
    PLXS_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_PLXS),
    /// Enable confirmation
    PLXS_ENABLE_RSP,

    /// Send pulse oximeter spot-check measurement from APP
    PLXS_SPOT_MEAS_SEND_REQ,
    /// Send pulse oximeter spot-check measurement response to APP
    PLXS_SPOT_MEAS_SEND_RSP,

    /// Send pulse oximeter continuous measurement from APP
    PLXS_CONT_MEAS_SEND_REQ,
    /// Send pulse oximeter continuous measurement response to APP
    PLXS_CONT_MEAS_SEND_RSP,

    // Send update of indication configuration for Spot-Check Measurement
    PLXS_SPOT_IND_CFG_IND,

    // Send update of notification configuration for Continuous Measurement
    PLXS_CONT_NTF_CFG_IND, 

    /// Inform APP that Indication Configuration has been changed - use to update bond data
    PLXS_CFG_INDNTF_IND,
};


/// Database Feature Configuration Flags
enum plxs_features
{
    /// Indicate if Spot-Check Measurement Characteristic is supported
    PLXS_SPOT_MEAS_CHAR_SUP        = 0x01,
    /// Indicate if Continuous Measurement Characteristic is supported
    PLXS_CONT_MEAS_CHAR_SUP        = 0x02,
    /// Indicate if PLX Features Characteristic is supported
    PLXS_FEAT_CHAR_SUP             = 0x04,
    /// Indicate if Record Access Control Point Characteristic is supported
    PLXS_RACP_CHAR_SUP             = 0x08,

    /// All Features supported
    PLXS_ALL_FEAT_SUP              = 0x0F,
};

/// Notification and indication configuration
enum plxs_ntf_ind_cfg
{
    /// Spot-Check Measurement indication enabled
    PLXS_CFG_SPOT_MEAS_IND    = (1 << 0),
    /// Continuous measurement notification enabled
    PLXS_CFG_CONT_MEAS_NTF    = (1 << 1),
};
/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */
/// Parameters of the Pulse Oximeter service database
struct plxs_db_cfg
{
    /// Pulse Oximeter Feature (@see enum plxs_features)
    uint8_t features;
};

/// Parameters of the @ref PLXS_ENABLE_REQ message
struct plxs_enable_req
{
    /// Connection index
    uint8_t conidx;
};

/// Parameters of the @ref PLXS_ENABLE_RSP message
struct plxs_enable_rsp
{
    /// Connection index
    uint8_t conidx;
    /// Status of enable request
    uint8_t status;
};

///Parameters of the @ref PLXS_SPOT_IND_CFG_IND message
struct plxs_spot_ind_cfg_ind
{
    /// connection index
    uint8_t  conidx;
    ///Indication Configuration
    uint8_t  ind_cfg;
};

///Parameters of the @ref PLXS_CONT_NTF_CFG_IND message
struct plxs_cont_ntf_cfg_ind
{
    /// connection index
    uint8_t  conidx;
    ///Notification Configuration
    uint8_t  ntf_cfg;
};

/// Parameters of the @ref PLXS_CFG_INDNTF_IND message
struct plxs_cfg_indntf_ind
{
    /// connection index
    uint8_t  conidx;
    /// Notification Configuration (@see enum plxs_ntf_ind_cfg)
    uint8_t  ntf_ind_cfg;
};

/// Parameters of the @ref PLXS_SPOT_MEAS_SEND_RSP message
struct plxs_spot_meas_send_rsp
{
    /// Status
    uint8_t status;
};

/// Parameters of the @ref PLXS_CONT_MEAS_SEND_RSP message
struct plxs_cont_meas_send_rsp
{
    /// Status
    uint8_t status;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
extern const struct ke_state_handler plxs_default_handler;

#endif //BLE_PLX_SERVER

/// @} PLXSTASK
#endif // PLXS_TASK_H_
