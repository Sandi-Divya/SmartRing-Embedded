/**
 ****************************************************************************************
 *
 * @file ctsc_task.h
 *
 * @brief Header file - CTSCTASK.
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

#ifndef CTSC_TASK_H_
#define CTSC_TASK_H_

/**
************************************************************************************
* @addtogroup CTSC Task
* @ingroup CTSC
* @brief Current Time Service Client Task
* @{
************************************************************************************
*/

/*
* INCLUDE FILES
************************************************************************************
*/

#if (BLE_CTS_CLIENT)
#include "prf_types.h"
#include "cts_common.h"
#include "ctsc.h"

/*
* DEFINES
****************************************************************************************
*/

/// Maximum number of Current Time Client task instances
#define CTSC_IDX_MAX        (BLE_CONNECTION_MAX)

/*
* TYPE DEFINITIONS
****************************************************************************************
*/

enum
{
    /// Not Connected State
    CTSC_FREE,
    /// Idle state
    CTSC_IDLE,
    /// Discovering State
    CTSC_DISCOVERING,
    /// Busy State
    CTSC_BUSY,

    /// Number of defined states.
    CTSC_STATE_MAX
};

enum ctsc_msg_id
{
    // Start the Current Time Service Client
    CTSC_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_CTSC),
    /// Confirm that cfg connection has finished with discovery results, or that normal cnx started
    CTSC_ENABLE_RSP,

    /// Message to read Current Time
    CTSC_READ_CT_REQ,
    /// Response for Current Time read request @ref CTSC_READ_CT_REQ
    CTSC_READ_CT_RSP,

    /// Message to read Local Time Information
    CTSC_READ_LTI_REQ,
    /// Response for Local Time read request @ref CTSC_READ_LTI_REQ
    CTSC_READ_LTI_RSP,

    /// Message to read Reference Time Information
    CTSC_READ_RTI_REQ,
    /// Response for Reference Time Information read request @ref CTSC_READ_RTI_REQ
    CTSC_READ_RTI_RSP,

    /// Message to read Characteristic Descriptor
    CTSC_READ_DESC_REQ,
    /// Response for Characteristic Descriptor read request @ref CTSC_READ_DESC_REQ
    CTSC_READ_DESC_RSP,

    /// Message to write Current Time
    CTSC_WRITE_CT_REQ,
    /// Response for Current Time write request @ref CTSC_WRITE_CT_REQ
    CTSC_WRITE_CT_RSP,

    /// Message to write Local Time Information
    CTSC_WRITE_LTI_REQ,
    /// Response for Local Time Information write request @ref CTSC_WRITE_LTI_REQ
    CTSC_WRITE_LTI_RSP,

    /// Message to write Characteristic Descriptor
    CTSC_WRITE_DESC_REQ,
    /// Response for Descriptor write request @ref CTSC_WRITE_DESC_REQ
    CTSC_WRITE_DESC_RSP,

    /// Current Time value send to APP
    CTSC_CT_IND,
};

/// Parameters of the @ref CTSC_ENABLE_REQ message
struct ctsc_enable_req
{
    /// Connection type
    uint8_t con_type;
    /// CTSS handle values and characteristic properties
    struct ctsc_cts_content cts;
};

/// Parameters of the @ref CTSC_ENABLE_RSP message
struct ctsc_enable_rsp
{
    /// Connection type
    uint8_t status;
    /// CTS handle values and characteristic properties
    struct ctsc_cts_content cts;
};

/// Parameters for the @ref CTSC_READ_CT_REQ,
//                     @ref CTSC_READ_LTI_REQ,
//                     @ref CTSC_READ_RTI_REQ,
//                     @ref CTSC_READ_DESC_REQ
struct ctsc_read_req
{
    /// Characteristic value code
    uint8_t  char_code;
};

/// Parameters of the @ref CTSC_READ_xxx_RSP and CTSC_WRITE_xxx_RSP
struct ctsc_rsp
{
    uint8_t status;
    uint8_t char_code;        /// Characteristic or descriptor
};


/// Parameters of the @ref CTSC_READ_CT_RSP message
struct ctsc_read_ct_rsp
{
    struct ctsc_rsp rsp;
    struct cts_curr_time ct_val;
};

/// Parameters of the @ref CTSC_READ_LTI_RSP message
struct ctsc_read_lti_rsp
{
    struct ctsc_rsp rsp;
    struct cts_loc_time_info lti_val;
};

/// Parameters of the @ref CTSC_READ_RTI_RSP message
struct ctsc_read_rti_rsp
{
    struct ctsc_rsp rsp;
    struct cts_ref_time_info rti_val;
};

/// Parameters for the @ref CTSC_READ_DESC_RSP message
struct ctsc_read_desc_rsp
{
    struct ctsc_rsp rsp;
    uint16_t ntf_cfg;
};

/// Parameters of the @ref CTSC_WRITE_xxx_REQ, message
struct ctsc_write_req
{
    uint8_t char_code;
};

/// Parameters of the @ref CTSC_WRITE_DESC_REQ message
struct ctsc_write_desc_req
{
    struct ctsc_write_req desc;
    uint16_t value;
};

/// Parameters of the @ref CTSC_WRITE_CT_REQ message
struct ctsc_write_ct_req
{
    struct ctsc_write_req chr;
    struct cts_curr_time ct_val;
};

/// Parameters of the @ref CTSC_WRITE_LTI_REQ message
struct ctsc_write_lti_req
{
    struct ctsc_write_req chr;
    struct cts_loc_time_info lti_val;
};

/// Parameters for the @ref CTSC_CT_IND message
struct ctsc_ct_ind
{
    uint8_t status;
    /// Current Time Value
    struct cts_curr_time ct_val;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
extern const struct ke_state_handler ctsc_default_handler;

#endif // BLE_CTS_CLIENT

/// @} CTSCTASK

#endif // CTSC_TASK_H_
