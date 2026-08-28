/**
 ****************************************************************************************
 *
 * @file cgms_task.h
 *
 * @brief Header file - Continuous Glucose Monitoring Profile Server.
 *
 * Copyright (C) 2022-2024 Renesas Electronics Corporation and/or its affiliates.
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

#ifndef CGMS_TASK_H_
#define CGMS_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup CGMSTASK Task
 * @ingroup CGMS
 * @brief Continuous Glucose Monitoring Profile Server Task
 *
 * The CGMSTASK is responsible for handling the messages coming in and out of the
 * @ref CGMS reporter block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#if (BLE_CGM_SERVER)
#include <stdint.h>
#include "ke_task.h"
#include "cgms.h"
#include "cgm_common.h"
#include "prf_types.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Possible states of the CGMS task
enum cgms_state
{
    /// Idle state
    CGMS_IDLE,
    /// Busy state
    CGMS_BUSY,

    /// Number of defined states.
    CGMS_STATE_MAX,
};

/// Messages for Continuous Glucose Monitoring Profile Server
enum cgms_msg_id
{
    /// Start the Continuous Glucose Monitoring Profile - at connection
    CGMS_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_CGMS),
    /// Enable confirmation
    CGMS_ENABLE_RSP,

    /// Send CGM Status request to APP
    CGMS_STATUS_SEND_REQ,
    /// Send CGM Status response from APP
    CGMS_STATUS_SEND_RSP,
    
    /// Send CGM Session Start Time request to APP
    CGMS_SESSION_START_TIME_REQ,
    /// Send CGM Session Start Time response from APP
    CGMS_SESSION_START_TIME_RSP,

    /// Inform APP that Indication Configuration has been changed - use to update bond data
    CGMS_CFG_INDNTF_IND,
     
    /// Record Access Control Point Request
    CGMS_RACP_REQ_RCV_IND,
    /// Record Access Control Point Resp
    CGMS_SEND_RACP_RSP_CMD,
    
    /// Specific Ops Control Point Request
    CGMS_SPEC_OPS_CP_REQ_RCV_IND,
    /// Specific Ops Control Point Resp
    CGMS_SEND_SPEC_OPS_CP_RSP_CMD,
    
    /// Send measurement command
    CGMS_SEND_MEAS_CMD,
    
    ///Inform that requested action has been performed
    CGMS_CMP_EVT,
};


/// Database Feature Configuration Flags
enum cgms_features
{
    /// All Features supported
    CGMS_ALL_FEAT_SUP              = 0xFF,
};

/// Notification and indication configuration
enum cgms_ntf_ind_cfg
{
    /// CGM Measurement notification enabled
    CGMS_CFG_MEASUREMENT_NTF    = (1 << 0),
    /// CGM Record Access Control Point indication enabled
    CGMS_CFG_RACP_IND           = (1 << 1),
    /// CGM Specific Ops Control Point indications enabled
    CGMS_CFG_SPEC_OPS_CP_IND    = (1 << 2),
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */
/// Parameters of the Continuous Glucose Monitoring Service database
struct cgms_db_cfg
{
    /// Continuous Glucose Monitoring Features (@see enum cgms_features)
    uint32_t features;
};

/// Parameters of the @ref CGMS_ENABLE_REQ message
struct cgms_enable_req
{
    /// Connection index
    uint8_t conidx;
};

/// Parameters of the @ref CGMS_ENABLE_RSP message
struct cgms_enable_rsp
{
    /// Connection index
    uint8_t conidx;
    /// Status of enable request
    uint8_t status;
};

/// Parameters of the @ref CGMS_STATUS_SEND_REQ message
struct cgms_status_send_req
{
    /// Connnection index
    uint8_t conidx;
};

/// Parameters of the @ref CGMS_STATUS_SEND_RSP message
struct cgms_status_send_rsp
{
    /// Connnection index
    uint8_t conidx;
    /// Sensor status
    struct cgms_status status;
};

///Parameters of the @ref CGMS_SESSION_START_TIME_REQ message
struct cgms_session_start_time_req
{
    /// Connection index
    uint8_t  conidx;
    
    ///RACP Request
    struct cgm_session_start_time session_start_time;
};

/// Parameters of the @ref CGMS_CFG_INDNTF_IND message
struct cgms_cfg_indntf_ind
{
    /// connection index
    uint8_t  conidx;
    /// Notification Configuration (@see enum cgms_ntf_ind_cfg)
    uint8_t  ntf_ind_cfg;
};

///Parameters of the @ref CGMS_RACP_REQ_RCV_IND message
struct cgms_racp_req_rcv_ind
{
    /// Connection index
    uint8_t  conidx;
    
    ///RACP Request
    struct cgm_racp_req racp_req;
};

///Parameters of the @ref CGMS_SPEC_OPS_CP_REQ_RCV_IND message
struct cgms_spec_ops_cp_req_rcv_ind
{
    /// Connection index
    uint8_t  conidx;
    
    ///Specific Ops Control Point Request
    struct cgm_spec_ops_cp_req spec_ops_cp_req;
};

///Parameters of the @ref CGMS_SEND_MEAS_CMD message
struct cgms_send_meas_cmd
{
    /// Glucose measurement
    struct cgm_meas meas;
};

///Parameters of the @ref CGMS_SEND_RACP_RSP_CMD message
struct cgms_send_racp_rsp_cmd
{
    ///Number of records.
    uint16_t num_of_record;
    /// operation code
    uint8_t op_code;
    ///Command status
    uint8_t status;
};

///Parameters of the @ref CGMS_CMP_EVT message
struct cgms_cmp_evt
{
    /// Completed request
    uint8_t request;
    /// Command status
    uint8_t status;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
extern const struct ke_state_handler cgms_default_handler;

#endif //BLE_CGM_SERVER

/// @} CGMSTASK
#endif // CGMS_TASK_H_
