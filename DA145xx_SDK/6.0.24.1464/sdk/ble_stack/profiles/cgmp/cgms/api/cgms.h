/**
 ****************************************************************************************
 *
 * @file cgms.h
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

#ifndef CGMS_H_
#define CGMS_H_

/**
 ****************************************************************************************
 * @addtogroup CGMS Continuos Glucose Monitoring Server
 * @ingroup CGM
 * @brief Continuous Glucose Monitoring Server
 *
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
#include <stdbool.h>
#include "atts.h"
#include "cgm_common.h"
#include "prf_types.h"
#include "prf_utils.h"

#include "gap.h"

/*
 * MACROS
 ****************************************************************************************
 */
#define CGMS_IS_FEATURE_SUPPORTED(feat, bit_mask) (((feat & bit_mask) == bit_mask))

#define CGMS_HANDLE(idx) (cgms_att_hdl_get(cgms_env, (idx)))

#define CGMS_IDX(hdl)    (cgms_att_idx_get(cgms_env, (hdl)))

/// Check if flag is enable
#define CGMS_IS(IDX, FLAG) ((cgms_env->flags[IDX] & (1<< (CGMS_##FLAG))) == (1 << (CGMS_##FLAG)))

/// Set flag enable
#define CGMS_SET(IDX, FLAG) (cgms_env->flags[IDX] |= (1 << (CGMS_##FLAG)))

/// Set flag disable
#define CGMS_CLEAR(IDX, FLAG) (cgms_env->flags[IDX] &= ~(1 << (CGMS_##FLAG)))

/*
 * DEFINES
 ****************************************************************************************
 */
///Maximum number of Continuous Glucose Monitoring Server task instances
#define CGMS_IDX_MAX    (1)

#define CGMS_SERV_DECL_MASK               (0x00000001)
#define CGMS_MEAS_CHAR_MASK               (0x0000000E)
#define CGMS_FEAT_CHAR_MASK               (0x00000030)
#define CGMS_STAT_CHAR_MASK               (0x000000C0)
#define CGMS_SES_START_TIME_CHAR_MASK     (0x00000300)
#define CGMS_SES_RUN_TIME_CHAR_MASK       (0x00000C00)
#define CGMS_RACP_CHAR_MASK               (0x00007000)
#define CGMS_SPEC_OPS_CHAR_MASK           (0x00038000)

#define CGMS_MEASUREMENT_ATT_NB           (3)
#define CGMS_FEAT_ATT_NB                  (2)
#define CGMS_STATUS_ATT_NB                (2)
#define CGMS_SESSION_START_TIME_ATT_NB    (2)
#define CGMS_SESSION_RUN_TIME_ATT_NB      (2)
#define CGMS_RACP_ATT_NB                  (3)
#define CGMS_SPEC_OPS_CP_ATT_NB           (3)

#define CGMS_FEAT_VALUE         (CGMS_FEAT_CALIBRATION_SUP | CGMS_FEAT_PATIENT_HL_ALERT_SUP | \
                                CGMS_FEAT_HYPO_ALERT_SUP | CGMS_FEAT_HYPER_ALERT_SUP | \
                                CGMS_FEAT_RATE_OF_INCR_DEACR_SUP | CGMS_FEAT_DEVICE_SPEC_ALERT_SUP |\
                                CGMS_FEAT_SENSOR_MALFUNC_DETECT_SUP | CGMS_FEAT_SENSOR_TEMP_HL_SUP |\
                                CGMS_FEAT_SENSOR_RESULT_HL_DETECT_SUP | CGMS_FEAT_LOW_BATTERY_DETECT_SUP |\
                                CGMS_FEAT_SENSOR_TYPE_ERR_DETECT_SUP | CGMS_FEAT_GEN_DEV_FAULT_DETECT_SUP |\
                                CGMS_FEAT_CGM_TREND_INFO_SUP | CGMS_FEAT_CGM_QUALITY_SUP)
                                
#define CGMS_FEAT_TYPE              (0x2)   
#define CGMS_FEAT_SAMPLE_LOC        (0x3)
#define CGMS_FEAT_TYPE_SAMPLE_LOC   ((CGMS_FEAT_SAMPLE_LOC << 4) | CGMS_FEAT_TYPE)

#define CGMS_SESSION_START_TIME_VAL_LENGTH      (9)

/*
 * ENUMS
 ****************************************************************************************
 */


 ///Attributes database elements
enum cgms_att_db_list
{
    CGM_IDX_SVC,

    CGM_IDX_MEASUREMENT_CHAR,
    CGM_IDX_MEASUREMENT_VAL,
    CGM_IDX_MEASUREMENT_NTF_CFG,

    CGM_IDX_FEAT_CHAR,
    CGM_IDX_FEAT_VAL,

    CGM_IDX_STATUS_CHAR,
    CGM_IDX_STATUS_VAL,

    CGM_IDX_SESSION_START_TIME_CHAR,
    CGM_IDX_SESSION_START_TIME_VAL,
    
    CGM_IDX_SESSION_RUN_TIME_CHAR,
    CGM_IDX_SESSION_RUN_TIME_VAL,
    
    CGM_IDX_RACP_CHAR,
    CGM_IDX_RACP_VAL,
    CGM_IDX_RACP_IND_CFG,
    
    CGM_IDX_SPEC_OPS_CP_CHAR,
    CGM_IDX_SPEC_OPS_CP_VAL,
    CGM_IDX_SPEC_OPS_CP_IND_CFG,
    
    CGM_IDX_NB,
};


/*
 * STRUCTS
 ****************************************************************************************
 */
 
 /// CGMS Processing Flags
enum
{
    /// Use To know if bond data are present
    CGMS_BOND_DATA_PRESENT,
    /// Flag used to know if there is an on-going record access control point request
    CGMS_RACP_ON_GOING,
    /// Service id sending a measurement
    CGMS_SENDING_MEAS,
    /// Measurement sent
    CGMS_MEAS_SENT,
    /// RACP Response sent by application
    CGMS_RACP_RSP_SENT_BY_APP,
    /// Session started
    CGMS_SPEC_OPS_CP_SESSION_STARTED,
    /// Session stopped
    CGMS_SPEC_OPS_CP_SESSION_STOPPED,    
};
 
/// Type of request completed
enum
{
    /// Glucose measurement notification sent completed
    CGMS_SEND_MEAS_REQ_NTF_CMP,
    /// Record Access Control Point Response Indication
    CGMS_SEND_RACP_RSP_IND_CMP,
    /// Special Ops Control Point Response Indication
    CGMS_SEND_SPEC_OPS_CP_RSP_IND_CMP,
}; 
 
/// Ongoing operation information
struct cgms_op
{
     /// Operation
     uint8_t op;
     /// Cursor on connection
     uint8_t cursor;
     /// Handle of the attribute to indicate/notify
     uint16_t handle;
     /// Task that request the operation that should receive completed message response
     uint16_t dest_id;
     /// Packed notification/indication data size
     uint8_t length;
     /// used to know on which device interval update has been requested, and to prevent
     /// indication to be triggered on this connection index
     uint8_t conidx;
     /// Packed notification/indication data
     uint8_t data[__ARRAY_EMPTY];
};

struct cgms_status
{
    uint16_t time_offset;
    uint32_t sensor_status_field;
};

struct cgms_session_run_time
{
    uint16_t time;
};

///Continuous Glucose Monitoring Profile Server Environment Variable
struct cgms_env_tag
{
    /// Profile environment
    prf_env_t prf_env;
    /// On-going operation
    struct cgms_op *operation;
    /// Service Start Handle
    uint16_t shdl;
    /// Database configuration
    uint32_t features;
    
    // Status field
    struct cgms_status status;
    // Session start time
    struct cgm_session_start_time session_start_time[BLE_CONNECTION_MAX];
    // Session run time
    struct cgms_session_run_time session_run_time[BLE_CONNECTION_MAX];
    
    /// Specific ops
    uint8_t cgms_comm_intv[BLE_CONNECTION_MAX];
    
    prf_sfloat cgms_patient_high_alert_lvl[BLE_CONNECTION_MAX];
    prf_sfloat cgms_patient_low_alert_lvl[BLE_CONNECTION_MAX];
    prf_sfloat cgms_hypo_alert_lvl[BLE_CONNECTION_MAX];
    prf_sfloat cgms_hyper_alert_lvl[BLE_CONNECTION_MAX];
    prf_sfloat cgms_rate_of_decr_alert_lvl[BLE_CONNECTION_MAX];
    prf_sfloat cgms_rate_of_incr_alert_lvl[BLE_CONNECTION_MAX];
    
    /// Flags set
    uint8_t flags[BLE_CONNECTION_MAX];
    /// Notification and indication configuration of peer devices.
    uint8_t ntf_ind_cfg[BLE_CONNECTION_MAX];
    /// State of different task instances
    ke_state_t state[CGMS_IDX_MAX];
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve CGM service profile interface
 *
 * @return CGM service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* cgms_prf_itf_get(void);

/**
 ****************************************************************************************
 * @brief Retrieve attribute handle from attribute index
 *
 * @param[in] cgms_env   Environment variable
 * @param[in] att_idx    Attribute Index
 *
 * @return attribute Handle
 ****************************************************************************************
 */
uint16_t cgms_att_hdl_get(struct cgms_env_tag* cgms_env, uint8_t att_idx);

/**
 ****************************************************************************************
 * @brief Retrieve attribute index from attribute handle
 *
 * @param[in] cgms_env   Environment variable
 * @param[in] handle     Attribute Handle
 *
 * @return attribute Index
 ****************************************************************************************
 */
uint8_t cgms_att_idx_get(struct cgms_env_tag* cgms_env, uint16_t handle);

/**
 ****************************************************************************************
 * @brief  This function fully manage notification and indication of continuous glucose monitoring
 *         to peer(s) device(s) according to on-going operation requested by application:
 ****************************************************************************************
 */
void cgms_exe_operation(void);

/**
 ****************************************************************************************
 * @brief Update Notification, Indication configuration
 *
 * @param[in] conidx    Connection index
 * @param[in] cfg       Indication configuration flag
 * @param[in] valid_val Valid value if NTF/IND enable
 * @param[in] value     Value set by peer device.
 *
 * @return status of configuration update
 ****************************************************************************************
 */
uint8_t cgms_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value);

/**
 ****************************************************************************************
 * @brief Pack CGMS Measurement value
 *
 * @param[out] packed_meas  Packed response
 * @param[in] meas_val      Unpacked measurement value
 *
 * @return Length of packed message
 ****************************************************************************************
 */
uint8_t cgms_pack_meas_value(uint8_t *packed_meas, const struct cgm_meas* meas_val);
/**
 ****************************************************************************************
 * @brief Unpack CGMS RACP request
 *
 * @param[in] packed_val    Packed request command
 * @param[in] length        Length of received request
 * @param[out] racp_req     Unpacked RACP request
 *
 * @return Status of unpack
 ****************************************************************************************
 */
uint8_t cgms_unpack_racp_req(const uint8_t *packed_val, uint16_t length, struct cgm_racp_req* racp_req);

/**
 ****************************************************************************************
 * @brief Pack CGMS RACP response
 *
 * @param[out] packed_val   Packed response
 * @param[in] racp_rsp      Unpacked RACP response
 *
 * @return Length of packed message
 ****************************************************************************************
 */
uint8_t cgms_pack_racp_rsp(uint8_t *packed_val, struct cgm_racp_rsp* racp_rsp);

/**
 ****************************************************************************************
 * @brief Unpack CGMS Specific Ops CP request
 *
 * @param[in] packed_val            Packed request command
 * @param[in] length                Length of received request
 * @param[out] spec_ops_cp_req      Unpacked Specific Ops CP request
 *
 * @return Status of unpack
 ****************************************************************************************
 */
uint8_t cgms_unpack_spec_ops_cp_req(const uint8_t *packed_val, uint16_t length, struct cgm_spec_ops_cp_req* spec_ops_cp_req);

/**
 ****************************************************************************************
 * @brief Pack CGMS Specific Ops CP response
 *
 * @param[out] packed_val           Packed response
 * @param[in]  spec_ops_cp_rsp      Unpacked Specific Ops CP response
 *
 * @return Length of packed message
 ****************************************************************************************
 */
uint8_t cgms_pack_spec_ops_cp_rsp(uint8_t *packed_val, struct cgm_spec_ops_cp_rsp* spec_ops_cp_rsp);

#endif //BLE_CGM_SERVER

/// @} CGMS

#endif // CGMS_H_
